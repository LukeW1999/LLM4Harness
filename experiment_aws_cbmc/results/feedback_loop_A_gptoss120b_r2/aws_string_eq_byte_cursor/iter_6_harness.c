#include <stdbool.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    /* Allocate and initialize a non‑deterministic aws_string on the stack */
    struct aws_string str;
    struct aws_string *str_ptr = &str;
    str_ptr->allocator = aws_default_allocator();

    ensure_string_has_allocated_buffer_member(str_ptr);

    size_t str_len = nondet_uint();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    str_ptr->len = str_len;

    for (size_t i = 0; i < str_len; ++i) {
        ((uint8_t *)str_ptr->bytes)[i] = nondet_uint8_t();
    }

    /* Allocate and initialize a non‑deterministic aws_byte_cursor */
    struct aws_byte_cursor cur;
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    size_t cur_len = nondet_uint();
    __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
    cur.len = cur_len;
    for (size_t i = 0; i < cur_len; ++i) {
        ((uint8_t *)cur.ptr)[i] = nondet_uint8_t();
    }

    /* Possibly make the pointers NULL */
    const struct aws_string *str_input = str_ptr;
    const struct aws_byte_cursor *cur_input = &cur;
    if (nondet_bool()) {
        str_input = NULL;
    }
    if (nondet_bool()) {
        cur_input = NULL;
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    if (str_input != NULL) {
        old_str = *str_input;
    }
    struct aws_byte_cursor old_cur = cur;

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str_input, cur_input);

    /* Post‑condition on return value */
    if (str_input == NULL && cur_input == NULL) {
        assert(result == true);
    } else if (str_input == NULL || cur_input == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str_input->bytes, str_input->len, cur_input->ptr, cur_input->len);
        assert(result == expected);
    }

    /* Unchanged fields when inputs are non‑NULL */
    if (str_input != NULL) {
        assert(str_input->allocator == old_str.allocator);
        assert(str_input->len == old_str.len);
        assert_bytes_match(str_input->bytes, old_str.bytes, str_input->len);
    }
    if (cur_input != NULL) {
        assert(cur_input->ptr == old_cur.ptr);
        assert(cur_input->len == old_cur.len);
        assert_bytes_match(cur_input->ptr, old_cur.ptr, cur_input->len);
    }

    /* Validity invariants */
    if (str_input != NULL) {
        assert(aws_string_is_valid(str_input));
    }
    if (cur_input != NULL) {
        assert(aws_byte_cursor_is_bounded(cur_input, MAX_BUFFER_SIZE));
    }
}
