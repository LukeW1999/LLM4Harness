#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC harness for aws_byte_buf_append */
void aws_byte_buf_append_harness(void) {
    /* Allocate and nondeterministically initialize the destination buffer */
    struct aws_byte_buf to;
    to.len = __CPROVER_nondet_size_t();
    to.capacity = __CPROVER_nondet_size_t();
    to.allocator = aws_default_allocator();

    __CPROVER_assume(to.capacity >= to.len);
    __CPROVER_assume(to.allocator != NULL);

    if (to.capacity > 0) {
        to.buffer = malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    /* Allocate and nondeterministically initialize the source cursor */
    struct aws_byte_cursor from;
    from.len = __CPROVER_nondet_size_t();

    if (from.len > 0) {
        from.ptr = malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /* Save old state for frame conditions */
    struct aws_byte_buf to_old = to;
    struct aws_byte_cursor from_old = from;

    /* Call the function under test */
    int ret = aws_byte_buf_append(&to, &from);

    /* ---- Postcondition checks ---- */

    /* Validity of destination buffer */
    assert(to.buffer != NULL || to.capacity == 0);
    assert(to.len <= to.capacity);
    assert(to.allocator == aws_default_allocator());

    /* Validity of source cursor */
    assert((from.len == 0) || from.ptr != NULL);

    /* Return value must be either success or the specific error */
    assert(ret == AWS_OP_SUCCESS ||
           ret == aws_raise_error(AWS_ERROR_DEST_COPY_TOO_SMALL));

    /* Length invariants */
    if (ret == AWS_OP_SUCCESS) {
        assert(to.len == to_old.len + from.len);
    } else {
        assert(to.len == to_old.len);
    }

    /* Frame conditions for fields that must not change */
    assert(to.capacity == to_old.capacity);
    assert(to.buffer == to_old.buffer);
    assert(to.allocator == to_old.allocator);
    assert(from.len == from_old.len);
    assert(from.ptr == from_old.ptr);

    /* If the call succeeded and data was copied, verify the copied bytes */
    if (ret == AWS_OP_SUCCESS && from.len > 0) {
        size_t i;
        for (i = 0; i < from.len; ++i) {
            assert(to.buffer[to_old.len + i] == from.ptr[i]);
        }
    }

    return 0;
}
