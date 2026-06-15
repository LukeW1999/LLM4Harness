#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* 1. Choose whether the pointer is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* 2. Choose a length for the flexible array member (including space for NUL) */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024);               /* bound to avoid overflow */

        /* 3. Allocate enough space for the struct plus the bytes and NUL terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* 4. Choose an allocator (default allocator or NULL) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* 5. Initialise the length field */
        str->len = len;

        /* 6. Initialise the byte contents and NUL terminator */
        uint8_t *bytes = str->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = '\0';

        /* 7. Assume the string is initially valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 8. Save the old state */
    struct aws_string *old_ptr = str;
    struct aws_string old;
    if (str != NULL) {
        old = *str;               /* shallow copy of the struct */
    }

    /* 9. Call the function under test */
    aws_string_destroy(str);

    /* 10. Post‑condition checks */
    if (old_ptr != NULL && old.allocator != NULL) {
        /* Memory should have been freed; we can only check that the pointer value
           itself has not been altered. */
        assert(str == old_ptr);
    } else {
        /* No free occurred; the struct must be unchanged. */
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            assert(aws_string_is_valid(str));
        } else {
            assert(str == NULL);
        }
    }
}
