/* CBMC harness for aws_byte_buf_clean_up */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include "proof_helpers/make_common_data_structures.h"

/* nondeterministic size_t */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_clean_up_harness(void) {
    /* allocator used by the buffer */
    struct aws_allocator *alloc = aws_default_allocator();

    /* --- set up a byte buffer with nondeterministic but bounded fields --- */
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    /* bound the capacity to avoid huge allocations in the proof */
    __CPROVER_assume(capacity <= 1024);

    if (capacity == 0) {
        buf.buffer = NULL;
    } else {
        /* allocate memory for the buffer; assume allocation succeeds */
        buf.buffer = (uint8_t *)aws_mem_acquire(alloc, capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* fill the allocated memory with nondeterministic data */
        for (size_t i = 0; i < capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    }

    buf.capacity = capacity;

    /* length must be <= capacity */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    buf.allocator = alloc;

    /* --- frame condition: allocate a separate region that must stay unchanged --- */
    uint8_t untouched[64];
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        untouched[i] = nondet_uint8_t();
    }
    uint8_t untouched_before[64];
    memcpy(untouched_before, untouched, sizeof(untouched));

    /* --- precondition: the buffer must be valid before the call --- */
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "precondition: buffer is valid");

    /* --- call the function under verification --- */
    aws_byte_buf_clean_up(&buf);

    /* --- postconditions --- */
    /* 1. The buffer must still satisfy its validity predicate */
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "postcondition: buffer remains valid");

    /* 2. All fields must be cleared as per the contract */
    __CPROVER_assert(buf.buffer == NULL, "postcondition: buffer pointer cleared");
    __CPROVER_assert(buf.allocator == NULL, "postcondition: allocator cleared");
    __CPROVER_assert(buf.len == 0, "postcondition: length cleared");
    __CPROVER_assert(buf.capacity == 0, "postcondition: capacity cleared");

    /* 3. Memory outside the buffer must be unchanged (frame condition) */
    __CPROVER_assert(
        memcmp(untouched, untouched_before, sizeof(untouched)) == 0,
        "frame condition: unrelated memory unchanged");

    return 0;
}
