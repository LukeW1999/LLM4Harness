#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Upper bound for nondeterministic capacity to keep the model tractable */
#define MAX_CAPACITY 1024

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    if (capacity > 0) {
        buf.buffer = aws_mem_acquire(alloc, capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* fill allocated memory with nondeterministic bytes */
        for (size_t i = 0; i < capacity; ++i) {
            ((uint8_t *)buf.buffer)[i] = nondet_uint8_t();
        }
    } else {
        buf.buffer = NULL;
    }

    /* nondeterministic length, must be <= capacity */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;
    buf.capacity = capacity;
    buf.allocator = alloc;

    /* Frame condition: some unrelated memory that must remain unchanged */
    uint8_t other[10];
    uint8_t other_before[10];
    for (size_t i = 0; i < sizeof(other); ++i) {
        other[i] = nondet_uint8_t();
    }
    memcpy(other_before, other, sizeof(other));

    /* Call the function under verification */
    aws_byte_buf_clean_up(&buf);

    /* Post‑conditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* Frame condition: unrelated memory unchanged */
    assert(memcmp(other, other_before, sizeof(other)) == 0);

    return 0;
}
