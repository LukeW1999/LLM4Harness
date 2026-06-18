#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Declare inputs */
    struct aws_string *str;
    struct aws_byte_cursor cur;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a nondeterministic length bounded by MAX_BUFFER_SIZE */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;               /* static string – allocator may be NULL */
        str->len = len;                       /* set length */
        /* the flexible array member `bytes` is part of the allocation; contents are nondet */
    }

    /* assume the string (if non‑NULL) satisfies its validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* prepare a byte cursor with allocated buffer and bounded size */
    ensure_byte_cursor_has_allocated_buffer_member(&cur);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    /* 2. Save old state for immutability checks */
    struct aws_string *old_str = NULL;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = malloc(sizeof(struct aws_string));
        __CPROVER_assume(old_str != NULL);
        *old_str = *str;                     /* copy allocator and len */
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_cursor old_cur = cur;
    struct store_byte_from_buffer old_cur_bytes;
    save_byte_from_array(cur.ptr, cur.len, &old_cur_bytes);

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* 4. Post‑condition assertions */

    /* If str is NULL, the function must return false (cur is never NULL here) */
    if (str == NULL) {
        assert(result == false);
    } else {
        /* result must equal the array‑equality of the underlying buffers */
        bool expected = aws_array_eq(str->bytes, str->len, cur.ptr, cur.len);
        assert(result == expected);
    }

    /* 5. Unchanged fields – string */
    if (str != NULL) {
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    /* 6. Unchanged fields – cursor */
    assert(cur.ptr == old_cur.ptr);
    assert(cur.len == old_cur.len);
    assert_byte_from_buffer_matches(cur.ptr, &old_cur_bytes);

    /* 7. Validity invariants after the call */
    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
