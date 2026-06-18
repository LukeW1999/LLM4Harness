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
    /* 1. Declare and bound the source buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    buffer.allocator = aws_default_allocator();

    /* 2. Nondeterministically choose capacity and length within bounds */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
    buffer.capacity = cap;

    size_t len_src = nondet_size_t();
    __CPROVER_assume(len_src <= cap);
    buffer.len = len_src;

    /* 3. Allocate the underlying buffer if needed */
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(buffer.len <= buffer.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 4. Declare and initialize the output buffer as an empty valid buffer */
    struct aws_byte_buf output;
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 5. Choose a non‑zero advance length that fits in the source buffer */
    size_t adv_len = nondet_size_t();
    __CPROVER_assume(adv_len > 0);
    __CPROVER_assume(adv_len <= buffer.len);
    __CPROVER_assume(adv_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buffer.buffer != NULL); /* source must have data when advancing */

    /* 6. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 7. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, adv_len);

    /* 8. The function must succeed under the above assumptions */
    assert(result);

    /* 9. Post‑condition checks for successful advance */
    assert(buffer.buffer == old_buffer.buffer + adv_len);
    assert(buffer.len == old_buffer.len - adv_len);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.len <= buffer.capacity);

    assert(output.buffer == old_buffer.buffer);
    assert(output.len == adv_len);
    assert(output.capacity == adv_len);
    assert(output.allocator == NULL);

    /* 10. Invariant: both structures remain valid */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
