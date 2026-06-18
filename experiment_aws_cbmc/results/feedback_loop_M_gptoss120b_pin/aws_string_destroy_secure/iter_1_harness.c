#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondet decide whether str is NULL or a valid string */
    if (nondet_bool()) {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate memory for the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondet decide whether the string has an allocator */
        if (nondet_bool()) {
            /* use the default allocator */
            *((struct aws_allocator * const *)&str->allocator) = aws_default_allocator();
        } else {
            *((struct aws_allocator * const *)&str->allocator) = NULL;
        }

        /* set the length (const field) */
        *((size_t const *)&str->len) = len;

        /* fill the bytes with nondet data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* Save old immutable state */
    struct aws_string old;
    if (str) {
        old = *str; /* copies allocator, len, and the bytes pointer */
    }

    /* Call the function under verification */
    aws_string_destroy_secure(str);

    /* Post‑condition checks */
    if (str) {
        /* Fields that must remain unchanged */
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);

        /* If the string had no allocator, it was not freed, so we can inspect the bytes */
        if (old.allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < str->len; ++i) {
                assert(bytes[i] == 0);
            }
            /* The bytes pointer itself must stay the same */
            assert((void *)bytes == (void *)aws_string_bytes(str));
        }

        /* The string must still satisfy its validity predicate */
        assert(aws_string_is_valid(str));
    } else {
        /* When str is NULL the function does nothing; no state to check */
    }
}
