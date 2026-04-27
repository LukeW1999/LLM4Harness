#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    const char *c_str;
    struct aws_byte_buf old_buf;

    // Initialize buf with allocated buffer and valid fields
    make_and_validate_byte_buf(&buf);
    old_buf = buf;

    // Assume c_str is a valid C string
    __CPROVER_assume(c_str != NULL && __CPROVER_r_ok(c_str, strlen(c_str) + 1));

    // Precondition: buf must be valid
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Call the function under test
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    // Frame conditions
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&buf));
}
