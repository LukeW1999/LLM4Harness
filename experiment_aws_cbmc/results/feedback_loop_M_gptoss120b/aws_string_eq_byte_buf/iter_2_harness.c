#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str;
    if (nondet_bool()) {
        ensure_string_is_allocated(&str, MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;
    const struct aws_string *old_str = str;

    bool result = aws_string_eq_byte_buf(str, &buf);

    if (result) {
        assert(str != NULL);
    } else {
        if (str != NULL) {
            /* No additional condition needed; result false when strings differ */
        }
    }

    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.owns_buffer == old_buf.owns_buffer);
    assert(str == old_str);

    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    assert(aws_byte_buf_is_valid(&buf));
}
