/* CBMC harness for aws_byte_buf_reset */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

#include <proof_helpers/make_common_data_structures.h>

/* nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_reset_harness(void) {
    /* bound the capacity to keep the verification tractable */
    const size_t MAX_CAPACITY = 64;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* allocate a raw region with guard bytes before and after the buffer */
    uint8_t *raw = NULL;
    if (capacity > 0) {
        raw = (uint8_t *)malloc(capacity + 2);
        __CPROVER_assume(raw != NULL);
    } else {
        /* still allocate guard bytes for the zero‑capacity case */
        raw = (uint8_t *)malloc(2);
        __CPROVER_assume(raw != NULL);
    }

    /* set guard bytes to nondeterministic values */
    uint8_t guard_before = nondet_uint8_t();
    uint8_t guard_after  = nondet_uint8_t();
    raw[0] = guard_before;
    raw[capacity + 1] = guard_after;

    /* construct the aws_byte_buf */
    struct aws_byte_buf buf;
    buf.buffer    = raw + 1;               /* point to the usable region */
    buf.capacity  = capacity;
    buf.len       = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* fill the buffer with nondeterministic data */
    for (size_t i = 0; i < buf.capacity; ++i) {
        buf.buffer[i] = nondet_uint8_t();
    }

    /* make a copy of the original contents for later comparison */
    uint8_t *orig = NULL;
    if (buf.capacity > 0) {
        orig = (uint8_t *)malloc(buf.capacity);
        __CPROVER_assume(orig != NULL);
        for (size_t i = 0; i < buf.capacity; ++i) {
            orig[i] = buf.buffer[i];
        }
    }

    /* nondeterministic flag for zeroing the contents */
    bool zero_contents = nondet_bool();

    /* call the function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* post‑conditions */

    /* the buffer must remain valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* length must be reset to zero */
    assert(buf.len == 0);

    /* capacity and allocator must be unchanged */
    assert(buf.capacity == capacity);
    assert(buf.allocator == aws_default_allocator());

    /* buffer pointer must be unchanged */
    assert(buf.buffer == raw + 1);

    /* guard bytes must be untouched */
    assert(raw[0] == guard_before);
    assert(raw[capacity + 1] == guard_after);

    if (zero_contents) {
        /* all bytes up to capacity must be zeroed */
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        /* contents must be unchanged */
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == orig[i]);
        }
    }

    /* clean up */
    free(orig);
    free(raw);
    return 0;
}
