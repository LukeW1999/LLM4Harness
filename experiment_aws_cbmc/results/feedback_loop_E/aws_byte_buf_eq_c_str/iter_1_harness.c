#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_c_str_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* parameters */
    const char *c_str;

    /* assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(c_str != NULL);

    /* save old state */
    struct aws_byte_buf old_buf = buf;

    /* call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* assertions */
    /* 1. Changed fields (from Doxygen) */
    /* No fields are explicitly mentioned as changing in the Doxygen for this function */

    /* 2. Unchanged fields (implied — Doxygen rarely lists these) */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);

    /* 3. Both return paths */
    /* For this function, there are no side effects and no failure path that changes the state */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* 4. Validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
