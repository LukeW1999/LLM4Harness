#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness() {
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    const struct aws_byte_buf *buf = NULL;

    struct aws_allocator *allocator = aws_default_allocator();

    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        str = ensure_aws_string_is_allocated(allocator, str_len);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        size_t buf_len = nondet_size_t();
        buf = ensure_aws_byte_buf_is_allocated(allocator, buf_len);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    (void)aws_string_eq_byte_buf(str, buf);
}
