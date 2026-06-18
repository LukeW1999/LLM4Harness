#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    if (is_null) {
        /* Test with NULL input — should be a no-op */
        aws_string_destroy_secure(NULL);
    } else {
        /* Test with a valid string using the aws allocator */
        struct aws_allocator *allocator = aws_default_allocator();

        size_t len;
        __CPROVER_assume(len <= 16);

        /* Allocate memory for the string header + data bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        struct aws_string *str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the fields using pointer casting to bypass const */
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
        *alloc_ptr = allocator;
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = len;

        /* Initialize bytes */
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t i = 0; i < len; i++) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0; /* null terminator */

        /* Verify the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Call the function under test */
        aws_string_destroy_secure(str);
    }
}
