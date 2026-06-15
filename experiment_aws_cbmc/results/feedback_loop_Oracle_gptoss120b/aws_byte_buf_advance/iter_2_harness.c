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

    /* ensure the advance length is within the available space */
    len = nondet_size_t();
    __CPROVER_assume(len <= buf.capacity - buf.len);

    /* save old state */
    size_t old_buf_len      = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_ptr    = buf.buffer;

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

    /* output‑buffer invariants and buffer state updates */
    assert(output.buffer == (old_buf_ptr == NULL ? NULL : old_buf_ptr + old_buf_len));
    assert(output.capacity == len);
    assert(output.len == 0);
    assert(aws_byte_buf_is_valid(&output));

    /* original buffer length is advanced, other fields unchanged */
    assert(buf.len == old_buf_len + len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.buffer == old_buf_ptr);
    assert(aws_byte_buf_is_valid(&buf));

    /* frame condition: underlying memory of the input buffer unchanged */
    if (buf_mem_copy != NULL) {
        assert(memcmp(buf.buffer, buf_mem_copy, buf.capacity) == 0);
        free(buf_mem_copy);
    }
}
