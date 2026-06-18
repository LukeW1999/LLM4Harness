#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);               /* bound the length */

        /* allocate the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + (len ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        /* fill the byte payload with nondeterministic data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save old state for later comparison (if str is non‑NULL) */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (str) {
        old = *str;                                 /* copy scalar fields */
        save_byte_from_array(aws_string_bytes(str), str->len, &old_bytes);
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Post‑conditions */
    if (str == NULL) {
        /* No effect when the input pointer is NULL */
        assert(str == NULL);
    } else {
        /* allocator and length must remain unchanged */
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);

        if (str->allocator == NULL) {
            /* For static strings the payload must be zeroed */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < str->len; ++i) {
                assert(bytes[i] == 0);
            }
            /* The string must still be valid after zeroing */
            assert(aws_string_is_valid(str));
        } else {
            /* For dynamically allocated strings the memory is released;
               we cannot inspect the payload after release, but the scalar
               fields must still be unchanged. */
            (void)old_bytes; /* silence unused‑variable warning */
        }
    }
}
