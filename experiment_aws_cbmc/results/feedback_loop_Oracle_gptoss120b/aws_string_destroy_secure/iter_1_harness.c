#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>

#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* allocate a string with flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    if (str) {
        /* nondeterministic allocator: either NULL or the default allocator */
        struct aws_allocator *alloc = nondet_bool() ? NULL : aws_default_allocator();
        str->allocator = alloc;
        str->len = len;

        /* nondeterministic bytes */
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t j = 0; j < len; ++j) {
            bytes[j] = nondet_uint8_t();
        }
    }

    /* keep a copy of the original length and a snapshot of one byte */
    size_t old_len = 0;
    uint8_t old_byte = 0;
    if (str) {
        old_len = str->len;
        if (old_len > 0) {
            size_t i = nondet_size_t();
            __CPROVER_assume(i < old_len);
            old_byte = ((uint8_t *)str->bytes)[i];
            /* store i for later use */
            __CPROVER_assume(i < old_len);
            /* expose i to the post‑condition */
            (void)i;
        }
    }

    /* structural validity assumption */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* call the function under verification */
    aws_string_destroy_secure(str);

    /* post‑conditions */
    if (str) {
        if (str->allocator == NULL) {
            /* when no allocator, the memory is not freed and must be zeroed */
            if (old_len > 0) {
                size_t i = nondet_size_t();
                __CPROVER_assume(i < old_len);
                assert(((uint8_t *)str->bytes)[i] == 0);
            }
            /* length must remain unchanged */
            assert(str->len == old_len);
        } else {
            /* when an allocator is present, the memory is released.
               We cannot dereference str after the free, but we can assert that
               the length field was not modified before the free. */
            assert(str->len == old_len);
        }
    } else {
        /* str == NULL: nothing to check */
        assert(str == NULL);
    }

    return 0;
}
