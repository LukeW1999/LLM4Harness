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
        /* 2. Choose a length for the flexible array member */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024); /* bound to avoid overflow */

        /* 3. Allocate enough space for the struct plus the bytes */
        size_t alloc_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* 4. Choose an allocator (NULL or the default allocator) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* 5. Initialise the length field */
        str->len = len;

        /* 6. Assume the string is initially valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 7. Save the old state */
    struct aws_string *old_ptr = str;
    struct aws_string old;
    if (str != NULL) {
        old = *str; /* shallow copy of the struct */
    }

    /* 8. Call the function under test */
    aws_string_destroy(str);

    /* 9. Post‑condition checks */
    if (old_ptr != NULL && old.allocator != NULL) {
        /* The memory has been freed; we can only check that the pointer value
           itself has not been altered. */
        assert(str == old_ptr);
        /* No further dereference of `str` is performed. */
    } else {
        /* No free occurred; the struct must be unchanged. */
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
        } else {
            assert(str == NULL);
        }
        /* The string must still satisfy its invariant. */
        assert(aws_string_is_valid(str));
    }
}
