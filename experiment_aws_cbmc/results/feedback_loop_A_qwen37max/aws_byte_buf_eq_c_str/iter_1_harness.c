#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.len, &old_byte);

    size_t c_str_cap = nondet_size_t();
    __CPROVER_assume(c_str_cap > 0 && c_str_cap <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_cap);
    __CPROVER_assume(c_str != NULL);
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < c_str_cap);
    c_str[c_str_len] = '\0';

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    if (buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_byte);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
