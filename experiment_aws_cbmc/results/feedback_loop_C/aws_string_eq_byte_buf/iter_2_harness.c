#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string str;
    struct aws_byte_buf buf;

    // Initialize str
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = bounded_malloc(str.len + 1); // +1 for null terminator
    if (str.bytes) {
        str.bytes[str.len] = '\0'; // Ensure null termination
    }

    // Initialize buf
    buf.buffer = bounded_malloc(nondet_size_t());
    buf.capacity = nondet_size_t();
    buf.len = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_ptr();

    // Ensure buf is bounded
    assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    // Save old states
    struct aws_string old_str = str;
    struct aws_byte_buf old_buf = buf;

    // Call the function under test
    bool result = aws_string_eq_byte_buf(&str, &buf);

    // Assert frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.allocator == old_buf.allocator);

    // Assert validity invariants
    assert(aws_string_is_valid(&str));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    // Assert postconditions based on the result
    if (result) {
        assert(str.len == buf.len);
        assert(memcmp(str.bytes, buf.buffer, str.len) == 0);
    } else {
        assert(str.len != buf.len || memcmp(str.bytes, buf.buffer, str.len) != 0);
    }
}
