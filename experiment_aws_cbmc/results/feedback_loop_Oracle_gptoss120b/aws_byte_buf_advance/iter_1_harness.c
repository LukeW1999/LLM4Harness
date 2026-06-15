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
    size_t len = nondet_size_t();

    /* structural pre‑conditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* save old state */
    size_t old_buf_len      = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_ptr    = buf.buffer;

    size_t old_output_len      = output.len;
    size_t old_output_capacity = output.capacity;
    uint8_t *old_output_ptr    = output.buffer;

    /* copy the underlying memory of the input buffer for frame‑condition check */
    uint8_t *buf_mem_copy = NULL;
    if (buf.buffer != NULL && buf.capacity > 0) {
        buf_mem_copy = malloc(buf.capacity);
        __CPROVER_assume(buf_mem_copy != NULL);
        memcpy(buf_mem_copy, buf.buffer, buf.capacity);
    }

    bool ret = aws_byte_buf_advance(&buf, &output, len);

    /* 1. return‑value correctness */
    if (old_buf_capacity - old_buf_len >= len && len <= SIZE_MAX / 2) {
        assert(ret == true);
    } else {
        assert(ret == false);
    }

    /* 2. output‑buffer invariants and buffer state updates */
    if (ret) {
        /* output must reference the tail of the original buffer */
        assert(output.buffer == (old_buf_ptr == NULL ? NULL : old_buf_ptr + old_buf_len));
        assert(output.capacity == len);
        assert(output.len == 0);

        /* original buffer length is advanced */
        assert(buf.len == old_buf_len + len);
        assert(buf.capacity == old_buf_capacity);
        assert(buf.buffer == old_buf_ptr);
    } else {
        /* on failure output is zeroed */
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.len == 0);

        /* original buffer unchanged */
        assert(buf.len == old_buf_len);
        assert(buf.capacity == old_buf_capacity);
        assert(buf.buffer == old_buf_ptr);
    }

    /* 3. frame condition: underlying memory of the input buffer unchanged */
    if (buf_mem_copy != NULL) {
        assert(memcmp(buf.buffer, buf_mem_copy, buf.capacity) == 0);
        free(buf_mem_copy);
    }

    return 0;
}
