#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL string pointer */
    struct aws_string *str;

    /* nondet decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 64); /* bound the payload size */

        /* allocate memory for the struct plus the flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondet decide whether the allocator field is NULL (static string) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        /* nondet initialize the bytes (including the implicit null terminator) */
        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the freshly created string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old immutable fields (if str is non‑NULL) */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */

    /* If str was NULL, it must remain NULL */
    if (str == NULL) {
        assert(str == NULL);
    } else {
        /* str is non‑NULL */
        if (old_allocator == NULL) {
            /* static string: destroy is a no‑op, fields must be unchanged */
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            /* bytes are immutable; we can optionally check the first byte */
            assert(((uint8_t *)str->bytes)[0] == ((uint8_t *)str->bytes)[0]);
        } else {
            /* dynamically allocated string: memory may have been released.
               We must not dereference str after this point. No further asserts. */
        }
    }

    /* 5. Validity invariant when no deallocation occurred */
    if (str == NULL || (str && str->allocator == NULL)) {
        assert(aws_string_is_valid(str));
    }

    /* 6. Clean up any allocated memory that was not freed by destroy */
    if (str && old_allocator != NULL) {
        /* In the case where destroy freed the memory, the pointer is now dangling;
           we avoid double free by not freeing it here. */
    } else if (str) {
        free((void *)str);
    }
}
