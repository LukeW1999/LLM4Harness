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

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    size_t src_capacity = nondet_size_t();
    __CPROVER_assume(src_capacity <= MAX_BUFFER_SIZE);
    size_t src_len = nondet_size_t();
    __CPROVER_assume(src_len <= src_capacity);

    src.capacity = src_capacity;
    src.len = src_len;
    src.allocator = aws_default_allocator();

    /* Allocate the buffer member if capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    /* Ensure the source buffer is valid and bounded */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* Save a byte from the source buffer for immutability check (if any) */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.capacity > 0) {
        save_byte_from_array(src.buffer, src.capacity, &src_byte);
    }

    /* Save old source state for later comparison */
    struct aws_byte_buf old_src = src;

    /* 2. Declare destination buffer (output) */
    struct aws_byte_buf dest;
    /* dest is uninitialized; the function will set its fields */

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, aws_default_allocator(), &src);

    /* 4. Postconditions for success path */
    if (result == AWS_OP_SUCCESS) {
        /* Allocator must be set to the one passed in */
        assert(dest.allocator == aws_default_allocator());

        /* Length and capacity must match source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* When source buffer is NULL, destination buffer must be NULL */
            assert(dest.buffer == NULL);
        } else {
            /* Destination must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* The buffer must be a different allocation from source */
            assert(dest.buffer != src.buffer);
            /* Contents must be identical for the length of the source */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* Destination must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* 5. Postconditions for failure path */
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Destination must still be a valid byte buffer (zeroed state) */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 6. Source buffer must be unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL && src.capacity > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    /* 7. Global validity invariants */
    assert(aws_byte_buf_is_valid(&src));
}
