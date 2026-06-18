#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256U

void aws_byte_cursor_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    /* Nondeterministic, but valid, initialization */
    ensure_byte_cursor_is_valid(&a, allocator);
    ensure_byte_buf_is_valid(&b, allocator);

    /* Preconditions: inputs must satisfy their validity predicates */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Additional assumptions required by the implementation */
    __CPROVER_assume((a.ptr != NULL) || (a.len == 0));
    __CPROVER_assume((b.buffer != NULL) || (b.len == 0));

    /* Bound lengths to avoid out‑of‑bounds reads in memcmp */
    __CPROVER_assume(a.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(b.len <= MAX_BUFFER_SIZE);

    /* Snapshot of input state */
    size_t a_len_snapshot = a.len;
    uint8_t *a_ptr_snapshot = a.ptr;
    size_t b_len_snapshot = b.len;
    uint8_t *b_buffer_snapshot = b.buffer;

    /* Call under verification */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Postcondition assertions */

    /* Validity of inputs must be preserved */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Frame conditions: structural fields unchanged */
    assert(a.len == a_len_snapshot);
    assert(a.ptr == a_ptr_snapshot);
    assert(b.len == b_len_snapshot);
    assert(b.buffer == b_buffer_snapshot);

    /* Frame conditions: memory contents unchanged */
    if (a_ptr_snapshot != NULL) {
        for (size_t i = 0; i < a_len_snapshot; ++i) {
            assert(a.ptr[i] == a_ptr_snapshot[i]);
        }
    }
    if (b_buffer_snapshot != NULL) {
        for (size_t i = 0; i < b_len_snapshot; ++i) {
            assert(b.buffer[i] == b_buffer_snapshot[i]);
        }
    }

    /* Result must reflect equality of length and byte‑wise contents */
    bool expected = true;
    if (a_len_snapshot != b_len_snapshot) {
        expected = false;
    } else {
        for (size_t i = 0; i < a_len_snapshot; ++i) {
            if (a_ptr_snapshot[i] != b_buffer_snapshot[i]) {
                expected = false;
                break;
            }
        }
    }
    assert(result == expected);
}
