#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str;
    const struct aws_byte_cursor *cur_ptr;
    struct aws_byte_cursor cur; // only used if cur_ptr is non-NULL

    /* Non-deterministic string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        /* Initialize fields (cast away const for harness) */
        *(struct aws_allocator **)(void *)&str->allocator = NULL;
        *(size_t *)(void *)&str->len = len;
        for (size_t i = 0; i < len; ++i) {
            *(uint8_t *)(void *)&str->bytes[i] = nondet_uint8_t();
        }
        /* Ensure null terminator at bytes[len] */
        *(uint8_t *)(void *)&str->bytes[len] = 0;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministic cursor pointer */
    if (nondet_bool()) {
        cur_ptr = NULL;
    } else {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        cur_ptr = &cur;
    }

    /* Save old state (only if non-NULL) */
    struct aws_byte_cursor old_cur;
    if (cur_ptr != NULL) {
        old_cur = *cur_ptr;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* Postconditions */

    /* 1. Immutability of inputs */
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_valid(cur_ptr));
        assert(cur_ptr->len == old_cur.len);
        assert(cur_ptr->ptr == old_cur.ptr);
    }
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    /* 2. Correctness based on NULL checks */
    if (str == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur_ptr == NULL) {
        assert(result == false);
    }
    /* When both non-NULL, the result depends on aws_array_eq which is out of scope. */
}
