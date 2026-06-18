#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str;

    if (nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough space for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the allocator is set */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        /* fill the byte payload with nondet data (including the implicit NUL) */
        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 2. Save old state for the “no‑free” case */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t *old_bytes = NULL;

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        if (old_len > 0) {
            old_bytes = malloc(old_len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
        }
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */

    /* Determine whether the function performed a free */
    bool performed_free = (str != NULL && str->allocator != NULL);

    if (!performed_free) {
        /* No free happened – the string (if any) must be unchanged */
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            if (old_len > 0) {
                assert_bytes_match(str->bytes, old_bytes, old_len);
            }
            /* The string must still satisfy its validity predicate */
            assert(aws_string_is_valid(str));
        } else {
            /* str was NULL – nothing to check */
        }
    } else {
        /* A free was performed.  The memory pointed to by `str` is now
         * deallocated, so we must not dereference it.  The only guarantee
         * we can make is that the function does not crash, which CBMC
         * already checks by construction.  No further assertions are
         * required for the freed case. */
    }

    /* 5. Clean‑up any auxiliary allocations */
    free(old_bytes);
    free(str);

    return 0;
}
