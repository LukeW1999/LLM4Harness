#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    size_t max_capacity;
    __CPROVER_assume(max_capacity <= MAX_BUFFER_SIZE);
    buf.buffer = malloc(max_capacity);
    __CPROVER_assume(buf.buffer != NULL);
    buf.capacity = max_capacity;
    buf.len = __CPROVER_nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_byte_buf_reset(&buf);

    /* 4. Assert postconditions for the function */
    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* Free allocated memory */
    free(buf.buffer);
}
