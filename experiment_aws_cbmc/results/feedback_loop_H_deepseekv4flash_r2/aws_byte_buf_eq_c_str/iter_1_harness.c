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
    /* Non-deterministic inputs */
    struct aws_byte_buf buf;
    const char *c_str = nondet_bool() ? NULL : (const char *)malloc(nondet_size_t());

    /* Assume buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call function */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postconditions */
    /* 1. Changed fields: none (function is read-only) */
    /* 2. Unchanged fields */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    /* 3. Both return paths: result is boolean, no failure path */
    /* 4. Validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
