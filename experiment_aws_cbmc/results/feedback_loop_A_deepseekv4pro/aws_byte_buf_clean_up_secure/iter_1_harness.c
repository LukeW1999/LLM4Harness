#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Assert postconditions */
    /* From Doxygen: "Equivalent to calling aws_byte_buf_secure_zero and then aws_byte_buf_clean_up on the buffer." */
    /* aws_byte_buf_secure_zero: sets all bytes of buffer to zero and resets len to zero */
    /* aws_byte_buf_clean_up: frees buffer, sets buffer=NULL, len=0, capacity=0, allocator=NULL */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
