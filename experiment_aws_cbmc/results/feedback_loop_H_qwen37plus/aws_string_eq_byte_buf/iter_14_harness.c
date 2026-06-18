#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    bool str_valid = nondet_bool();
    if (str_valid) {
        str = ensure_string_is_allocated();
    }

    struct aws_byte_buf buf;
    bool buf_valid = nondet_bool();
    if (buf_valid) {
        ensure_byte_buf_is_valid(&buf);
        ensure_byte_buf_has_allocated_buffer_member(&buf);
    }

    const struct aws_string *str_ptr = str;
    const struct aws_byte_buf *buf_ptr = buf_valid ? &buf : NULL;

    bool result = aws_string_eq_byte_buf(str_ptr, buf_ptr);
    (void)result;

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    if (buf_valid) {
        assert(aws_byte_buf_is_valid(&buf));
    }
}
