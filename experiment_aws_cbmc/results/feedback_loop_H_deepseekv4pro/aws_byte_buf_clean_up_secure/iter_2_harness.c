#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep state space manageable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Set allocator non-deterministically: either the default allocator or NULL */
    buf.allocator = nondet_bool() ? aws_default_allocator() : NULL;

    /* Re-validate after setting allocator */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for postcondition checks */
    struct aws_byte_buf old = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postcondition: length is always zero */
    assert(buf.len == 0);

    /* Postcondition: allocator becomes NULL */
    assert(buf.allocator == NULL);

    /* Postcondition: buffer pointer becomes NULL, capacity becomes 0 */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);

    /* If the old buffer was non-NULL and had an allocator, the data should have been zeroed before freeing */
    if (old.allocator != NULL && old.buffer != NULL) {
        for (size_t i = 0; i < old.capacity; i++) {
            assert(old.buffer[i] == 0);
        }
    }

    /* Postcondition: the buffer remains valid after the call (zeroed struct is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
