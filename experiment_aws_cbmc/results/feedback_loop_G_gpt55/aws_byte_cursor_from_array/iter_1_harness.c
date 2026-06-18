#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    struct aws_byte_cursor input;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&input, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&input);
    __CPROVER_assume(aws_byte_cursor_is_valid(&input));

    struct aws_byte_cursor old = input;

    struct aws_byte_cursor result = aws_byte_cursor_from_array(input.ptr, input.len);

    assert(result.ptr == old.ptr);
    assert(result.len == old.len);

    assert(input.ptr == old.ptr);
    assert(input.len == old.len);

    if (old.len == 0) {
        assert(result.len == 0);
    } else {
        assert(result.ptr != NULL);
        assert(AWS_MEM_IS_READABLE(result.ptr, result.len));
    }

    assert(aws_byte_cursor_is_valid(&input));
    assert(aws_byte_cursor_is_valid(&result));
}
