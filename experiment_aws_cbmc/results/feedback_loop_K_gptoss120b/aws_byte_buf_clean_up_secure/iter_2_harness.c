/* Harness for aws_byte_buf_clean_up_secure */

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stddef.h>

/* Nondeterministic generators */
size_t nondet_size_t(void);
uint8_t *nondet_uint8_ptr(void);

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= SIZE_MAX / 2);

    /* Initialize buffer */
    int init_res = aws_byte_buf_init(&buf, alloc, capacity);
    __CPROVER_assume(init_res == 0);

    /* Nondeterministic length within capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Ensure pre‑condition validity */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Preserve original allocator for later checks */
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Post‑conditions */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    /* allocator may be cleared by the function */
    assert(buf.allocator == NULL || buf.allocator == old_allocator);
    assert(aws_byte_buf_is_valid(&buf));
}
