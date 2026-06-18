#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* nondet decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate enough space for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* initialise fields (allocator may be NULL for static strings) */
        str->allocator = NULL;
        str->len = len;

        /* the bytes are part of the allocation; their contents are nondet */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide whether cur is NULL */
    if (nondet_bool()) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = NULL;
    struct aws_byte_cursor old_cur;
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string));
        __CPROVER_assume(old_str != NULL);
        *old_str = *str;
        /* save the byte contents of the string */
        save_byte_from_array(str->bytes, str->len, (struct store_byte_from_buffer *)0);
    }
    if (cur != NULL) {
        old_cur = *cur;
        /* save the byte contents of the cursor's buffer */
        save_byte_from_array(cur->ptr, cur->len, (struct store_byte_from_buffer *)0);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Postconditions for return value */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
        assert(result == expected);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_bytes_match(str->bytes, old_str->bytes, str->len);
    }
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        assert_bytes_match(cur->ptr, old_cur.ptr, cur->len);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* clean up */
    free(str);
    free(cur);
    free(old_str);
}
