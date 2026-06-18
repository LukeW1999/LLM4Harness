#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);

    buf.capacity = capacity;
    buf.len = len;
    buf.allocator = aws_default_allocator();

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct store_byte_from_buffer saved_byte;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &saved_byte);
    }

    struct aws_byte_buf old_buf = buf;

    bool zero = nondet_bool();

    aws_byte_buf_reset(&buf, zero);

    /* Buffer metadata must remain unchanged except for length */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == 0);

    /* If zeroing is not requested, the saved byte must be unchanged */
    if (!zero && buf.buffer != NULL && buf.capacity > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &saved_byte);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
