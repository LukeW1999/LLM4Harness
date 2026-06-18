#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* CBMC nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_clean_up_harness(void) {
    /* Use the default allocator for all allocations */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic capacity for the buffer, bounded to keep the model tractable */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);

    /* Initialize the buffer; assume successful allocation */
    struct aws_byte_buf buf;
    int init_res = aws_byte_buf_init(&buf, alloc, capacity);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* Nondeterministic length, must not exceed capacity */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    /* Fill the allocated memory with nondeterministic data */
    if (capacity > 0) {
        for (size_t i = 0; i < capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    }

    /* --------------------------------------------------------------------- */
    /* Frame condition: allocate a separate region that must remain unchanged */
    size_t other_size = 64;
    uint8_t other[64];
    uint8_t other_copy[64];
    for (size_t i = 0; i < other_size; ++i) {
        other[i] = nondet_uint8_t();
    }
    memcpy(other_copy, other, other_size);
    /* --------------------------------------------------------------------- */

    /* Verify precondition */
    assert(aws_byte_buf_is_valid(&buf));

    /* Call the function under verification */
    aws_byte_buf_clean_up(&buf);

    /* -------------------------- Postconditions -------------------------- */
    /* 1. The buffer must still satisfy its validity predicate */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. All fields must be cleared as per the contract */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* 3. No memory outside the buffer's contract may have been modified */
    assert(memcmp(other, other_copy, other_size) == 0);

    return 0;
}
