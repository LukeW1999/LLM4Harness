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

    /* CBMC's memcmp model requires non‑NULL pointers even for zero length */
    __CPROVER_assume(a.buffer != NULL);
    __CPROVER_assume(b.buffer != NULL);

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
