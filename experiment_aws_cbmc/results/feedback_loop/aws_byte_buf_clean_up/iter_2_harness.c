#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;

    /* 2. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 3. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 4. Call function under test */
    aws_byte_buf_clean_up(&buf);

    /* 5. Assert postconditions */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
}
