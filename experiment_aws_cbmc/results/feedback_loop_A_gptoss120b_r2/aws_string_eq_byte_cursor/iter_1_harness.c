#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* Allocate and initialize a non‑deterministic aws_string */
    struct aws_string *str_alloc = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE - 1);
    __CPROVER_assume(str_alloc != NULL);
    str_alloc->allocator = aws_default_allocator();               /* const field, ok in harness */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    *((size_t *)&str_alloc->len) = str_len;                       /* cast away const */
    for (size_t i = 0; i < str_len; ++i) {
        ((uint8_t *)str_alloc->bytes)[i] = nondet_uint8_t();
    }

    /* Allocate and initialize a non‑deterministic aws_byte_cursor */
    struct aws_byte_cursor cur;
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    size_t cur_len = nondet_size_t();
    __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
    cur.len = cur_len;
    for (size_t i = 0; i < cur_len; ++i) {
        ((uint8_t *)cur.ptr)[i] = nondet_uint8_t();
    }

    /* Possibly make the pointers NULL */
    const struct aws_string *str = str_alloc;
    struct aws_byte_cursor *cur_ptr = &cur;
    if (nondet_bool()) {
        str = NULL;
    }
    if (nondet_bool()) {
        cur_ptr = NULL;
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }
    struct aws_byte_cursor old_cur = cur;
    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* Post‑condition on return value */
    if (str == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur_ptr == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur_ptr->ptr, cur_ptr->len);
        assert(result == expected);
    }

    /* Unchanged fields when inputs are non‑NULL */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_str.bytes, str->len);
    }
    if (cur_ptr != NULL) {
        assert(cur_ptr->ptr == old_cur.ptr);
        assert(cur_ptr->len == old_cur.len);
        assert_bytes_match(cur_ptr->ptr, old_cur.ptr, cur_ptr->len);
    }

    /* Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_bounded(cur_ptr, MAX_BUFFER_SIZE));
    }

    /* Clean up */
    free(str_alloc);
}
