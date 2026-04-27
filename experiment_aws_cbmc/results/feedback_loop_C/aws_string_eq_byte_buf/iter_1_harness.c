// === STEP 1: SUCCESS PATH ===
// When aws_string_eq_byte_buf returns true:
//   - str.len: UNCHANGED
//   - str.bytes: UNCHANGED
//   - buf.len: UNCHANGED
//   - buf.buffer: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq_byte_buf returns false:
//   - str.len: UNCHANGED
//   - str.bytes: UNCHANGED
//   - buf.len: UNCHANGED
//   - buf.buffer: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// buf (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - capacity: UNCHANGED always
//   - len: UNCHANGED always
//   - allocator: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): YES (must hold after call)
//   - aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE): YES (must hold after call)

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
}
