#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet length and capacity within bounds */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= MAX_BUFFER_SIZE);

    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.capacity >= buf.len);

    /* allocate buffer if capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* set allocator and assume validity */
    buf.allocator = alloc;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* preserve old state for potential later checks */
    struct aws_byte_buf old_buf = buf;

    /* function under test */
    aws_byte_buf_clean_up(&buf);

    /* post‑conditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
