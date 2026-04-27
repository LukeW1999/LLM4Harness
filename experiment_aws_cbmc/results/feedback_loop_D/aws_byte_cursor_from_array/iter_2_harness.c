#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_array_harness() {
    const void *bytes = (const void *)nondet_ptr();
    size_t len = nondet_size_t();

    // Assume bytes is a valid pointer and len is within bounds for the buffer
    __CPROVER_assume(__CPROVER_r_ok(bytes, len));

    struct aws_byte_cursor cur;
    cur.ptr = (uint8_t *)nondet_ptr();
    cur.len = nondet_size_t();

    cur = aws_byte_cursor_from_array(bytes, len);

    // Frame conditions
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&cur));
}
