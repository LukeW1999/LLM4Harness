#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    /* nondeterministically decide whether the buffer has an allocated backing array */
    if (nondet_bool()) {
        /* allocate a capacity within the allowed bound */
        size_t cap = nondet_size_t();
        __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
        buf.buffer = (uint8_t *)malloc(cap);
        __CPROVER_assume(buf.buffer != NULL);
        buf.capacity = cap;
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);
    } else {
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
    }
    /* allocator can be NULL for the purposes of this harness */
    buf.allocator = NULL;

    /* ensure the constructed buffer satisfies the library's validity predicate */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* preserve the original state for later comparison */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &buf_storage);
    }

    /* function under test */
    aws_byte_buf_reset(&buf);

    /* post‑conditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (buf.buffer != NULL && buf.capacity > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &buf_storage);
    }
}
