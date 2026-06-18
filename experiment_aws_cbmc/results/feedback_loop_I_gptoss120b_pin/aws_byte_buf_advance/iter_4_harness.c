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
    struct aws_byte_buf output;
    size_t len;

    /* 2. Set up a bounded, valid source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 3. Set up an empty, valid destination buffer */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 4. Choose a nondet length that fits within the source buffer */
    len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= buffer.len);   /* must be advanceable */

    /* 5. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 6. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 7. The call must succeed because len <= buffer.len */
    assert(result == true);

    /* 8. Post‑conditions for a successful advance */
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.buffer == old_buffer.buffer + len);
    assert(buffer.len == old_buffer.len - len);

    assert(output.capacity == len);
    assert(output.len == len);
    assert(output.buffer == old_buffer.buffer);
    assert(output.allocator == NULL);

    /* 9. Invariants must still hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
