#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    char c_str[MAX_BUFFER_SIZE + 1];
    __CPROVER_assume(c_str[MAX_BUFFER_SIZE] == '\0');

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));
    assert(aws_byte_buf_is_valid(&buf));
}
