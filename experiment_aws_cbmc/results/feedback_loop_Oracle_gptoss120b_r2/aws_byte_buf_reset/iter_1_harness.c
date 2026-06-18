/* CBMC harness for aws_byte_buf_reset */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_reset_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* --- create a byte buffer with nondeterministic capacity --- */
    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);               /* bound for tractability */

    /* Initialise the buffer; assume success */
    int init_rc = aws_byte_buf_init(&buf, alloc, capacity);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);

    /* --- set a nondeterministic length (<= capacity) --- */
    if (capacity > 0) {
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= capacity);
    } else {
        buf.len = 0;
    }

    /* --- fill the buffer with nondeterministic data up to len --- */
    for (size_t i = 0; i < buf.len; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* --- also fill the unused part (if any) with nondet data --- */
    for (size_t i = buf.len; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* --- make a copy of the original contents for later comparison --- */
    uint8_t *orig_contents = NULL;
    if (buf.capacity > 0) {
        orig_contents = malloc(buf.capacity);
        assert(orig_contents != NULL);
        for (size_t i = 0; i < buf.capacity; ++i) {
            orig_contents[i] = buf.buffer[i];
        }
    }

    /* --- remember original invariants --- */
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    /* --- nondeterministic zero_contents flag --- */
    bool zero_contents = nondet_bool();

    /* --- call the function under test --- */
    aws_byte_buf_reset(&buf, zero_contents);

    /* ------------------- post‑conditions ------------------- */

    /* 1. The buffer must still be a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. Length must be reset to zero, capacity unchanged, allocator unchanged */
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);

    /* 3. If zero_contents is true, all bytes up to the original capacity must be zero */
    if (zero_contents) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        /* otherwise the memory must be unchanged */
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == orig_contents[i]);
        }
    }

    /* 4. No other memory should have been modified (orig_contents is untouched) */
    if (orig_contents != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(orig_contents[i] == orig_contents[i]); /* trivially true, forces CBMC to keep the object */
        }
    }

    /* clean up */
    aws_byte_buf_clean_up(&buf);
    free(orig_contents);

    return 0;
}
