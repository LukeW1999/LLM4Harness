#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

_Bool nondet_bool(void);

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* make both buffer‑present and buffer‑absent cases reachable */
    if (nondet_bool()) {
        __CPROVER_assume(buf.buffer != NULL);
        __CPROVER_assume(buf.capacity > 0);
    } else {
        buf.buffer = NULL;
        buf.capacity = 0;
    }

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
