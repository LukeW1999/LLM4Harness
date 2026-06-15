#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/string.h>

/* nondeterministic helpers */
extern _Bool nondet_bool(void);
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* 1. Allocate a string (or NULL) nondeterministically */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (or NULL) */
        if (nondet_bool()) {
            str->allocator = malloc(sizeof(struct aws_allocator));
            __CPROVER_assume(str->allocator != NULL);
        } else {
            str->allocator = NULL;
        }

        /* set length (const field) */
        *((size_t *)&str->len) = len;

        /* initialise payload */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Preserve old state */
    struct aws_string *old_str = str;
    size_t old_len = (str) ? str->len : 0;
    uint8_t *old_bytes = NULL;
    if (str) {
        old_bytes = malloc(old_len);
        __CPROVER_assume(old_bytes != NULL || old_len == 0);
        for (size_t i = 0; i < old_len; ++i) {
            old_bytes[i] = ((uint8_t *)aws_string_bytes(str))[i];
        }
    }

    /* 3. Call function under test */
    aws_string_destroy_secure(str);

    /* 4. Post‑condition checks */
    /* pointer value must stay the same */
    assert(str == old_str);

    if (old_str) {
        if (old_str->allocator == NULL) {
            /* memory should be zeroed */
            const uint8_t *bytes = aws_string_bytes(old_str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
            /* still a valid string (zero‑length payload) */
            assert(aws_string_is_valid(old_str));
        } else {
            /* allocator present: memory may have been freed; we cannot dereference */
            /* no further dereference assertions */
        }
    } else {
        /* str was NULL: nothing to check beyond pointer equality */
    }
}
