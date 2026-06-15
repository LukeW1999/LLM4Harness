#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_styles.h>
#include <proof_helpers/proof_allocators.h>
#include <stdlib.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* Limit sizes to bound CBMC loops */
    size_t a_len;
    size_t b_len;
    __CPROVER_assume(a_len <= 10);
    __CPROVER_assume(b_len <= 10);

    struct aws_byte_cursor a;
    struct aws_byte_buf b;

    /* Allocate buffers if lengths > 0 */
    uint8_t *a_buf = NULL;
    uint8_t *b_buf = NULL;
    if (a_len > 0) {
        a_buf = malloc(a_len);
        __CPROVER_assume(a_buf != NULL);
        for (size_t i = 0; i < a_len; ++i) {
            a_buf[i] = nondet_uint8_t();
        }
    }
    if (b_len > 0) {
        b_buf = malloc(b_len);
        __CPROVER_assume(b_buf != NULL);
        for (size_t i = 0; i < b_len; ++i) {
            b_buf[i] = nondet_uint8_t();
        }
    }

    a.len = a_len;
    a.ptr = a_buf;
    b.len = b_len;
    b.buffer = b_buf;
    b.capacity = b_len;
    b.allocator = NULL; /* or nondet, but NULL is safe */

    /* Ensure validity assumptions (simplified) */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state */
    size_t old_a_len = a.len;
    uint8_t *old_a_ptr = a.ptr;
    size_t old_b_len = b.len;
    uint8_t *old_b_buffer = b.buffer;
    size_t old_b_capacity = b.capacity;
    void *old_b_allocator = b.allocator;

    /* Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Assert unchanged fields (const arguments → no modification) */
    assert(a.len == old_a_len);
    assert(a.ptr == old_a_ptr);
    assert(b.len == old_b_len);
    assert(b.buffer == old_b_buffer);
    assert(b.capacity == old_b_capacity);
    assert(b.allocator == old_b_allocator);

    /* Assert validity invariants */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Behavioral correctness: result == (lengths equal AND bytes match) */
    if (a.len == b.len) {
        bool byte_match = true;
        for (size_t i = 0; i < a.len; ++i) {
            if (a.ptr[i] != b.buffer[i]) {
                byte_match = false;
                break;
            }
        }
        if (byte_match) {
            assert(result);
        } else {
            assert(!result);
        }
    } else {
        assert(!result);
    }

    /* Free allocated buffers */
    free(a_buf);
    free(b_buf);
}
