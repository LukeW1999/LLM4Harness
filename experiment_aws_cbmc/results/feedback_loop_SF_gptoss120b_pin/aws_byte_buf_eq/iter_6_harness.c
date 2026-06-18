#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_eq_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* nondeterministic, but valid, byte buffers */
    ensure_byte_buf_is_allocated(&a, alloc);
    ensure_byte_buf_is_allocated(&b, alloc);

    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* ensure length does not exceed capacity (strengthen the validity assumption) */
    __CPROVER_assume(a.len <= a.capacity);
    __CPROVER_assume(b.len <= b.capacity);

    /* non‑NULL buffers when length is non‑zero (required by CBMC's memcmp model) */
    __CPROVER_assume(a.len == 0 || a.buffer != NULL);
    __CPROVER_assume(b.len == 0 || b.buffer != NULL);

    /* make the buffer contents defined for memcmp */
    if (a.len > 0) {
        make_nondet_data(a.buffer, a.len);
    }
    if (b.len > 0) {
        make_nondet_data(b.buffer, b.len);
    }

    /* PRE‑CALL SNAPSHOT */
    size_t a_len_snapshot = a.len;
    size_t b_len_snapshot = b.len;
    uint8_t *a_buf_snapshot = a.buffer;
    uint8_t *b_buf_snapshot = b.buffer;

    /* CALL */
    bool result = aws_byte_buf_eq(&a, &b);

    /* POST‑CONDITIONS */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    assert(a.len == a_len_snapshot);
    assert(b.len == b_len_snapshot);
    assert(a.buffer == a_buf_snapshot);
    assert(b.buffer == b_buf_snapshot);

    bool expected = false;
    if (a_len_snapshot == b_len_snapshot) {
        if (a_len_snapshot == 0) {
            expected = true;
        } else {
            expected = (0 == memcmp(a.buffer, b.buffer, a_len_snapshot));
        }
    }
    assert(result == expected);
}
