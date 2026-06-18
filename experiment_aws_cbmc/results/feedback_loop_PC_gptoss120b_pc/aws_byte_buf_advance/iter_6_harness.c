#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output = {0}; /* must be zeroed per precondition */

    /* 2. Bound and allocate the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(buffer.buffer != NULL);               /* non‑NULL backing array */
    __CPROVER_assume(buffer.len <= buffer.capacity);       /* invariant */

    /* 3. Nondeterministic length to advance */
    size_t len = nondet_size_t();

    /* 4. Constrain len so that the function can succeed */
    __CPROVER_assume(len <= buffer.len);                    /* must be within current data */

    /* 5. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct store_byte_from_buffer old_buffer_bytes = {0};
    if (buffer.buffer != NULL) {
        save_byte_from_array(buffer.buffer, buffer.capacity, &old_buffer_bytes);
    }

    /* 6. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 7. Assert post‑conditions */
    if (result) {
        /* Success: source buffer advanced */
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(buffer.allocator == old_buffer.allocator);

        /* Output buffer describes the advanced region */
        const uint8_t *expected_ptr = old_buffer.buffer;
        assert(output.buffer == expected_ptr);
        assert(output.capacity == len);
        assert(output.len == len);
        assert(output.allocator == NULL);
    } else {
        /* Failure: source buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* 8. Underlying memory must not be modified */
    if (buffer.buffer != NULL) {
        assert_byte_from_buffer_matches(buffer.buffer, &old_buffer_bytes);
    }

    /* 9. Invariants: structures remain valid */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
