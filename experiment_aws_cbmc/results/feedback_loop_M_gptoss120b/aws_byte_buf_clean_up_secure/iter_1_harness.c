#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and nondeterministically initialize the byte buffer */
    struct aws_byte_buf buf;

    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Allocate the buffer member according to the (possibly nondet) capacity */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Nondeterministically decide whether the buffer has an allocator */
    if (nondet_bool()) {
        buf.allocator = aws_default_allocator();
    } else {
        buf.allocator = NULL;
    }

    /* Ensure the structure is initially valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save the old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Post‑condition assertions */
    /* The function zeroes the buffer and then frees it, leaving the struct empty */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);

    /* The allocator field must remain unchanged */
    assert(buf.allocator == old.allocator);

    /* 5. The resulting structure must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
