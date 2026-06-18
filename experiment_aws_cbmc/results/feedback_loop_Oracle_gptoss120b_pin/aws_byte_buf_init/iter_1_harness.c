#include <aws/common/byte_buf.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers */
size_t nondet_uint64(void);
uint8_t nondet_uint8(void);

void aws_byte_buf_init_harness(void) {
    /* Allocate the buffer structure */
    struct aws_byte_buf buf;

    /* Nondeterministic old buffer state */
    size_t old_capacity = nondet_uint64();
    __CPROVER_assume(old_capacity <= 1024);               /* bound allocation size */

    uint8_t *old_buffer = NULL;
    if (old_capacity > 0) {
        old_buffer = malloc(old_capacity);
        __CPROVER_assume(old_buffer != NULL);
        for (size_t i = 0; i < old_capacity; ++i) {
            old_buffer[i] = nondet_uint8();
        }
    }

    buf.buffer   = old_buffer;
    buf.capacity = old_capacity;
    buf.len      = nondet_uint64();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = NULL;   /* will be set by the function */

    /* Snapshot of the old buffer contents for frame condition */
    uint8_t *old_content = NULL;
    if (old_buffer != NULL) {
        old_content = malloc(old_capacity);
        __CPROVER_assume(old_content != NULL);
        for (size_t i = 0; i < old_capacity; ++i) {
            old_content[i] = old_buffer[i];
        }
    }

    /* Nondeterministic capacity argument for the function */
    size_t capacity = nondet_uint64();
    __CPROVER_assume(capacity <= 1024);   /* bound allocation size */

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Call the function under test */
    int ret = aws_byte_buf_init(&buf, allocator, capacity);

    /* Postcondition 1: Return value / error code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* Success case: buffer must be valid */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* Failure case: buffer must be zeroed */
        assert(buf.len == 0);
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    /* Postcondition 2: Output buffer length/capacity invariants */
    if (ret == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    }

    /* Postcondition 3: Frame conditions (memory not modified beyond contract) */
    if (old_buffer != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(old_buffer[i] == old_content[i]);
        }
    }

    /* Clean up */
    if (old_buffer != NULL) {
        free(old_buffer);
    }
    if (old_content != NULL) {
        free(old_content);
    }
    if (ret == AWS_OP_SUCCESS && buf.buffer != NULL && capacity != 0) {
        /* The function allocated memory; free it to avoid leaks in the harness */
        aws_mem_release(allocator, buf.buffer);
    }

    return 0;
}
