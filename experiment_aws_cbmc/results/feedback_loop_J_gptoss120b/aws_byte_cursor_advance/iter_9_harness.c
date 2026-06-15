#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_cursor cursor = {
        .ptr = buf.buffer,
        .len = buf.len
    };

    const uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;
    struct aws_byte_buf old_buf = buf;

    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_storage);
    }

    size_t amount = nondet_size_t();
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(amount <= old_len, "amount within old length");
        __CPROVER_assert(cursor.ptr == old_ptr + amount, "ptr advanced correctly");
        __CPROVER_assert(cursor.len == old_len - amount, "len reduced correctly");
        if (cursor.len > 0) {
            assert_bytes_match(buf.buffer + amount, cursor.ptr, cursor.len);
        }
    } else {
        __CPROVER_assert(cursor.ptr == old_ptr, "ptr unchanged on failure");
        __CPROVER_assert(cursor.len == old_len, "len unchanged on failure");
    }

    __CPROVER_assert(buf.buffer == old_buf.buffer, "buffer pointer unchanged");
    __CPROVER_assert(buf.len == old_buf.len, "buffer len unchanged");
    __CPROVER_assert(buf.capacity == old_buf.capacity, "buffer capacity unchanged");
    __CPROVER_assert(buf.allocator == old_buf.allocator, "buffer allocator unchanged");

    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buffer remains valid");

    __CPROVER_assert(
        (cursor.ptr == NULL && cursor.len == 0) ||
        (cursor.ptr >= buf.buffer &&
         cursor.ptr <= buf.buffer + buf.len &&
         cursor.len <= (size_t)(buf.buffer + buf.len - cursor.ptr)),
        "cursor stays within buffer bounds");
}
