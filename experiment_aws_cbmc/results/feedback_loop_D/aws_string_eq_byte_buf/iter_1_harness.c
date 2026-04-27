// === STEP 1: SUCCESS PATH ===
// When aws_string_eq_byte_buf returns true:
//   - str->bytes: UNCHANGED
//   - str->len: UNCHANGED
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq_byte_buf returns false:
//   - str->bytes: UNCHANGED
//   - str->len: UNCHANGED
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED

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
//   - aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE): YES/NO (assumed bounded for this harness)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = malloc(sizeof(*str));
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_buf *buf = malloc(sizeof(*buf));
    ensure_byte_buf_has_allocated_buffer_member(buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));

    struct aws_string old_str = *str;
    struct aws_byte_buf old_buf = *buf;

    bool result = aws_string_eq_byte_buf(str, buf);

    // Frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes == old_str.bytes);
    assert(buf->buffer == old_buf.buffer);
    assert(buf->capacity == old_buf.capacity);
    assert(buf->len == old_buf.len);
    assert(buf->allocator == old_buf.allocator);

    // Validity invariants
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
}
