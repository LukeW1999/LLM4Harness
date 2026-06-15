#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Bound the buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Nondeterministically decide whether the buffer is allocated */
    if (nondet_bool()) {
        /* Allocate a buffer and set allocator */
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        buf.allocator = alloc;
    } else {
        /* No allocation */
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for frame condition checks */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_bytes;
    if (buf.buffer != NULL) {
        save_byte_from_array(buf.buffer, buf.capacity, &old_bytes);
    }

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Postcondition: buffer fields must be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Validity predicate must hold after cleanup */
    assert(aws_byte_buf_is_valid(&buf));

    /* Frame condition: original buffer pointer (if any) is no longer reachable */
    if (old_buf.buffer != NULL) {
        assert(buf.buffer != old_buf.buffer);
        /* Ensure the original bytes have not been altered (they were freed) */
        assert_byte_from_buffer_matches(old_buf.buffer, &old_bytes);
    }

    /* The original buffer structure (except for fields that are allowed to change) should not be otherwise modified */
    /* Since all fields are expected to be zeroed, no additional unchanged asserts are required */
}
