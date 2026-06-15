#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;

    /* Choose whether to model a non‑empty buffer or an empty one */
    if (nondet_bool()) {
        size_t cap = nondet_size_t();
        __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
        uint8_t *mem = (uint8_t *)malloc(cap);
        __CPROVER_assume(mem != NULL);
        buf.buffer   = mem;
        buf.capacity = cap;
        buf.len      = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);

        struct aws_allocator *alloc = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        __CPROVER_assume(alloc != NULL);
        buf.allocator = alloc;
    } else {
        buf.buffer   = NULL;
        buf.capacity = 0;
        buf.len      = 0;
        buf.allocator = NULL;
    }

    /* Preserve original state for later comparison */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &buf_storage);
    }

    /* Call the function under verification */
    aws_byte_buf_reset(&buf);

    /* Post‑conditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (buf.buffer != NULL && buf.capacity > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &buf_storage);
    }
}
