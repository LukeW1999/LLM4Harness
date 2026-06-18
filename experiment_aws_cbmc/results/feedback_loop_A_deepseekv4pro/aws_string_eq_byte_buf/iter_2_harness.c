#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministically decide if arguments are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* Allocator for dynamic allocations */
    struct aws_allocator *allocator = aws_default_allocator();

    /* If str is not NULL, create a valid dynamic string */
    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        /* ensure_aws_string_is_allocated allocates a string with given length */
        str = ensure_aws_string_is_allocated(allocator, str_len);
        /* Fill the string bytes with non-deterministic values */
        if (str != NULL && str_len > 0) {
            for (size_t i = 0; i < str_len; i++) {
                str->bytes[i] = nondet_uint8_t();
            }
        }
    }

    /* If buf is not NULL, create a valid byte buffer */
    if (!buf_is_null) {
        size_t buf_len = nondet_size_t();
        /* ensure_aws_byte_buf_is_allocated allocates a buffer with given length */
        buf = ensure_aws_byte_buf_is_allocated(allocator, buf_len);
        /* Fill the buffer with non-deterministic values */
        if (buf != NULL && buf_len > 0) {
            for (size_t i = 0; i < buf_len; i++) {
                buf->buffer[i] = nondet_uint8_t();
            }
        }
    }

    /* Call the function under test */
    (void)aws_string_eq_byte_buf(str, buf);
}
