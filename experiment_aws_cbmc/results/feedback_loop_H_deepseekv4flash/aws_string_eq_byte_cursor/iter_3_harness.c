#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_cursor cur;
    const struct aws_byte_cursor *cur_ptr = NULL;

    /* Non-deterministic string: either NULL or a valid string */
    if (nondet_bool()) {
        /* NULL string */
        str = NULL;
    } else {
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        /* Initialize fields */
        *(struct aws_allocator **)(void *)&str->allocator = (struct aws_allocator *)1;
        *(size_t *)(void *)&str->len = len;
        for (size_t i = 0; i < len; ++i) {
            *(uint8_t *)(void *)&str->bytes[i] = nondet_uint8_t();
        }
        /* Null terminate */
        *(uint8_t *)(void *)&str->bytes[len] = 0;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministic cursor pointer: either NULL or a valid cursor */
    if (nondet_bool()) {
        cur_ptr = NULL;
    } else {
        /* Initialize cursor to a bounded, valid value */
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur));
        cur_ptr = &cur;
    }

    /* Save old state for postcondition checks */
    struct aws_byte_cursor old_cur;
    if (cur_ptr != NULL) {
        old_cur = *cur_ptr;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    /* Postcondition: inputs unchanged and valid */
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_valid(cur_ptr));
        assert(cur_ptr->len == old_cur.len);
        assert(cur_ptr->ptr == old_cur.ptr);
    }
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    /* Postcondition: correctness based on NULL checks */
    if (str == NULL && cur_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur_ptr == NULL) {
        assert(result == false);
    }
    /* When both non-NULL, result is determined by aws_array_eq - not checked further */
}
