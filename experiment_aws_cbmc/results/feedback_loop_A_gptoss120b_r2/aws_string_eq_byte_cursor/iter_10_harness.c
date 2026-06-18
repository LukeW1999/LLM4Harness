#include <stdbool.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_cursor_harness(void) {
    /* Non‑deterministic string length and contents */
    size_t str_len = nondet_uint();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    uint8_t str_buf[MAX_BUFFER_SIZE];
    for (size_t i = 0; i < str_len; ++i) {
        str_buf[i] = nondet_uint8_t();
    }

    /* Allocate an aws_string from the buffer */
    struct aws_string *str = aws_string_new_from_array(aws_default_allocator(), str_buf, str_len);
    __CPROVER_assume(str != NULL);

    /* Non‑deterministic cursor length and contents */
    size_t cur_len = nondet_uint();
    __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
    uint8_t cur_buf[MAX_BUFFER_SIZE];
    for (size_t i = 0; i < cur_len; ++i) {
        cur_buf[i] = nondet_uint8_t();
    }

    /* Initialise the aws_byte_cursor */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(cur_buf, cur_len);

    /* Possibly make the pointers NULL */
    const struct aws_string *str_input = str;
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
        bool expected = false;
        if (str_input->len == cur_input->len) {
            expected = true;
            for (size_t i = 0; i < str_input->len; ++i) {
                if (str_input->bytes[i] != ((const uint8_t *)cur_input->ptr)[i]) {
                    expected = false;
                    break;
                }
            }
        }
        assert(result == expected);
    }

    /* Unchanged fields when inputs are non‑NULL (excluding byte contents) */
    if (str_input != NULL) {
        assert(str_input->allocator == old_str.allocator);
        assert(str_input->len == old_str.len);
    }
    if (cur_input != NULL) {
        assert(cur_input->ptr == old_cur.ptr);
        assert(cur_input->len == old_cur.len);
    }

    /* Validity invariants */
    if (str_input != NULL) {
        assert(aws_string_is_valid(str_input));
    }
    if (cur_input != NULL) {
        assert(aws_byte_cursor_is_bounded(cur_input, MAX_BUFFER_SIZE));
    }

    /* Clean up */
    if (str != NULL) {
        aws_string_destroy(str);
    }
}
