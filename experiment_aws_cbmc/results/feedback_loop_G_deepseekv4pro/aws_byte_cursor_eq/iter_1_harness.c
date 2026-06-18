#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_eq_harness() {
    /* Non-deterministic cursor lengths (bounded to limit state space) */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Declare cursors and set their lengths */
    struct aws_byte_cursor cursor_a;
    struct aws_byte_cursor cursor_b;
    cursor_a.len = len_a;
    cursor_b.len = len_b;

    /* Allocate non‑deterministic backing memory for both cursors */
    ensure_byte_cursor_has_allocated_buffer_member(&cursor_a);
    ensure_byte_cursor_has_allocated_buffer_member(&cursor_b);

    /* Assume the cursors are valid (pointers readable for their lengths) */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor_a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor_b));

    /* Save old state of cursors (including pointer values) */
    struct aws_byte_cursor old_a = cursor_a;
    struct aws_byte_cursor old_b = cursor_b;

    /* Save the byte content of the backing arrays for later comparison */
    struct store_byte_from_buffer stored_a;
    struct store_byte_from_buffer stored_b;
    save_byte_from_array(cursor_a.ptr, cursor_a.len, &stored_a);
    save_byte_from_array(cursor_b.ptr, cursor_b.len, &stored_b);

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(&cursor_a, &cursor_b);

    /* ----- Postconditions ----- */

    /* 1. Input cursors must be completely unchanged */
    assert(cursor_a.len == old_a.len);
    assert(cursor_a.ptr == old_a.ptr);
    assert(cursor_b.len == old_b.len);
    assert(cursor_b.ptr == old_b.ptr);

    /* 2. Backing buffers must not be modified */
    assert_byte_from_buffer_matches(cursor_a.ptr, &stored_a);
    assert_byte_from_buffer_matches(cursor_b.ptr, &stored_b);

    /* 3. Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&cursor_a));
    assert(aws_byte_cursor_is_valid(&cursor_b));

    /* 4. Semantic consistency: if the function returns true, the two cursors
     *    must have equal length and byte‑for‑byte identical content. */
    if (result) {
        assert(cursor_a.len == cursor_b.len);
        assert_bytes_match(cursor_a.ptr, cursor_b.ptr, cursor_a.len);
    }
}
