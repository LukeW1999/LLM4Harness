#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_eq_harness(void) {
    /* Symbolic inputs */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    a.len = nondet_size_t();
    b.len = nondet_size_t();

    a.ptr = (uint8_t *)malloc(a.len);
    b.ptr = (uint8_t *)malloc(b.len);

    __CPROVER_assume((a.len == 0) || (a.ptr != NULL));
    __CPROVER_assume((b.len == 0) || (b.ptr != NULL));

    /* Pre‑call snapshot */
    size_t a_len_snapshot = a.len;
    size_t b_len_snapshot = b.len;
    uint8_t *a_ptr_snapshot = a.ptr;
    uint8_t *b_ptr_snapshot = b.ptr;

    uint8_t *a_contents_snapshot = NULL;
    uint8_t *b_contents_snapshot = NULL;

    if (a_len_snapshot > 0) {
        a_contents_snapshot = (uint8_t *)malloc(a_len_snapshot);
        __CPROVER_assume(a_contents_snapshot != NULL);
        memcpy(a_contents_snapshot, a_ptr_snapshot, a_len_snapshot);
    }

    if (b_len_snapshot > 0) {
        b_contents_snapshot = (uint8_t *)malloc(b_len_snapshot);
        __CPROVER_assume(b_contents_snapshot != NULL);
        memcpy(b_contents_snapshot, b_ptr_snapshot, b_len_snapshot);
    }

    /* Call under verification */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* ASSERT_POSTCONDITIONS_HERE */
}
