#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "aws/common/byte_buf.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

/* nondet helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
void *nondet_pointer(void);

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = nondet_pointer();
    __CPROVER_assume(alloc == NULL || alloc == aws_default_allocator());
    buf.allocator = alloc;

    /* nondet capacity within bound */
    size_t cap = nondet_size_t();
    __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
    buf.capacity = cap;

    /* nondet length respecting capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    if (buf.allocator != NULL) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    } else {
        /* buffer may be constant memory; we do not dereference it */
        buf.buffer = nondet_pointer();
    }

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* frame condition: memory outside the buffer must stay unchanged */
    uint8_t *outside = malloc(10);
    __CPROVER_assume(outside != NULL);
    for (int i = 0; i < 10; ++i) {
        outside[i] = nondet_uint8_t();
    }
    uint8_t outside_before[10];
    memcpy(outside_before, outside, 10);

    /* save original state */
    struct aws_allocator *orig_alloc = buf.allocator;
    uint8_t *orig_buffer = buf.buffer;
    size_t orig_len = buf.len;
    size_t orig_cap = buf.capacity;

    /* function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    if (orig_alloc != NULL) {
        /* buffer should be freed and pointer cleared */
        assert(buf.buffer == NULL);
    } else {
        /* buffer pointer unchanged, but contents zeroed */
        assert(buf.buffer == orig_buffer);
        if (orig_buffer != NULL && orig_cap > 0) {
            for (size_t i = 0; i < orig_cap; ++i) {
                assert(orig_buffer[i] == 0);
            }
        }
    }
    /* allocator must remain unchanged */
    assert(buf.allocator == orig_alloc);

    /* verify frame condition */
    for (int i = 0; i < 10; ++i) {
        assert(outside[i] == outside_before[i]);
    }

    return 0;
}
