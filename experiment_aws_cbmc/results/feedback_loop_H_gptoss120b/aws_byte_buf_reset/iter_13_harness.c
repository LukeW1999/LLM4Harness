#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

#define MAX_BUFFER_SIZE 1024

/* Nondeterministic helpers */
size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;

    /* Choose whether to model a non‑empty buffer or an empty one */
    if (nondet_bool()) {
        size_t cap = nondet_size_t();
        __CPROVER_assume(cap <= MAX_BUFFER_SIZE && cap > 0);
        uint8_t *mem = (uint8_t *)malloc(cap);
        __CPROVER_assume(mem != NULL);
        buf.buffer   = mem;
        buf.capacity = cap;
        buf.len      = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);
        buf.allocator = NULL; /* allocator not used by reset */
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
    assert(buf.allocator == old_buf.allocator);

    if (old_buf.buffer != NULL) {
        /* Non‑empty original buffer: buffer and capacity must stay unchanged */
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
        if (buf.buffer != NULL && buf.capacity > 0) {
            assert_byte_from_buffer_matches(buf.buffer, &buf_storage);
        }
    } else {
        /* Empty original buffer: buffer stays NULL and capacity stays 0 */
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
    }
}
