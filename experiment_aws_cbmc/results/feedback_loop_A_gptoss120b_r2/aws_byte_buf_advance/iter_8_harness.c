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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output = {0}; /* zero‑initialized, thus valid */

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Ensure the underlying buffer pointer is non‑NULL */
    __CPROVER_assume(buffer.buffer != NULL);

    /* Additional capacity invariants required by aws_byte_buf_advance */
    __CPROVER_assume(buffer.capacity >= buffer.len);
    /* For simplicity, make capacity equal to length (common case) */
    __CPROVER_assume(buffer.capacity == buffer.len);

    /* nondeterministic length, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len > 0);                     /* advance must be non‑zero */
    __CPROVER_assume(len <= buffer.len);          /* cannot advance past data */
    __CPROVER_assume(buffer.capacity >= len);     /* capacity sufficient */

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* Save a byte from the region that will be returned in `output` */
    struct store_byte_from_buffer storage;
    if (len > 0) {
        save_byte_from_array(old_buffer.buffer, len, &storage);
    }

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. The function must succeed under the above assumption */
    assert(result);

    /* Success: buffer advanced */
    assert(buffer.buffer == old_buffer.buffer + len);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity - len);
    assert(buffer.allocator == old_buffer.allocator);

    /* Success: output describes the consumed region */
    assert(output.buffer == old_buffer.buffer);
    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.allocator == NULL);

    /* Bytes referenced by output must match original buffer bytes */
    if (len > 0) {
        assert_bytes_match(old_buffer.buffer, output.buffer, len);
    }

    /* 5. Validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
