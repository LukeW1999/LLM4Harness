#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buf;
    struct aws_byte_buf output;
    size_t len;

    /* allocate and initialize the source buffer */
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    /* allocator is left nondeterministic */
    buf.allocator = nondet_pointer();

    /* ensure the advance length is within the available space and non‑zero */
    len = nondet_size_t();
    __CPROVER_assume(len > 0);
    __CPROVER_assume(len <= buf.capacity - buf.len);

    /* initialize output as a valid (empty) buffer */
    output.buffer = NULL;
    output.capacity = 0;
    output.len = 0;
    output.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* save old state */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_byte_from_buf;
    save_byte_from_array(buf.buffer, buf.len, &old_byte_from_buf);
    struct aws_allocator *old_buf_allocator = buf.allocator;

    /* copy the underlying memory of the input buffer for frame‑condition check */
    uint8_t *buf_mem_copy = NULL;
    if (buf.buffer != NULL && buf.capacity > 0) {
        buf_mem_copy = malloc(buf.capacity);
        __CPROVER_assume(buf_mem_copy != NULL);
        memcpy(buf_mem_copy, buf.buffer, buf.capacity);
    }

    bool ret = aws_byte_buf_advance(&buf, &output, len);

    /* the function must succeed under the above assumptions */
    assert(ret == true);

    /* expert post‑conditions for the successful case */
    assert(buf.len == old.len + len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old_buf_allocator);
    if (old.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_byte_from_buf);
    }
    assert(output.len == 0);
    assert(output.capacity == len);
    assert(output.allocator == NULL);

    /* original harness checks */
    assert(output.buffer == (old.buffer == NULL ? NULL : old.buffer + old.len));
    assert(aws_byte_buf_is_valid(&output));

    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(aws_byte_buf_is_valid(&buf));

    /* frame condition: underlying memory of the input buffer unchanged */
    if (buf_mem_copy != NULL) {
        assert(memcmp(buf.buffer, buf_mem_copy, buf.capacity) == 0);
        free(buf_mem_copy);
    }

    /* final validity checks */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_valid(&output));
}
