#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* nondeterministic, but valid, byte buffers */
    ensure_byte_buf_is_allocated(&a, alloc);
    ensure_byte_buf_is_allocated(&b, alloc);

    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* PRE‑CALL SNAPSHOT */
    size_t a_len_snapshot = a.len;
    size_t b_len_snapshot = b.len;
    uint8_t *a_buf_snapshot = a.buffer;
    uint8_t *b_buf_snapshot = b.buffer;

    /* CALL */
    bool result = aws_byte_buf_eq(&a, &b);

    /* ASSERT_POSTCONDITIONS_HERE */
}
