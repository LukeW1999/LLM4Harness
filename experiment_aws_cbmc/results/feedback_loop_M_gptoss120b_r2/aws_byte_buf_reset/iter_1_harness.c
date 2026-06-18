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

void aws_byte_buf_init_harness(void) {
    /* 1. Declare the buffer and bound its fields */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 2. Choose a nondeterministic capacity and bound it */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Use the default allocator (always available) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Save the old state for immutability checks (if needed) */
    struct aws_byte_buf old = buf;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init(&buf, alloc, capacity);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* On success the buffer must be initialized as described */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == alloc);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* On failure the buffer is zeroed out */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
        /* The old state is not preserved on failure (it is zeroed) */
    }

    /* 7. The buffer must always satisfy its validity predicate */
    assert(aws_byte_buf_is_valid(&buf));
}
