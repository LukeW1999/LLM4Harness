#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. nondet string pointer (may be NULL) */
    const struct aws_string *str;
    bool str_is_null = nondet_bool();

    if (str_is_null) {
        str = NULL;
    } else {
        /* nondet length bounded */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate raw memory for flexible‑array struct */
        uint8_t *mem = malloc(sizeof(struct aws_string) + (len ? len - 1 : 0));
        __CPROVER_assume(mem != NULL);
        /* initialize the const fields via a temporary struct and memcpy */
        struct aws_string tmp = {
            .allocator = aws_default_allocator(),
            .len       = len
        };
        memcpy(mem, &tmp, sizeof(struct aws_string));

        str = (const struct aws_string *)mem;
    }

    /* Assume the string (if non‑NULL) satisfies the library’s validity check */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state of the string (if any) */
    const struct aws_string *old_str = str;
    struct store_byte_from_buffer str_store;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &str_store);
    }

    /* 2. nondet byte_cursor pointer (may be NULL) */
    const struct aws_byte_cursor *cur;
    bool cur_is_null = nondet_bool();

    if (cur_is_null) {
        cur = NULL;
    } else {
        struct aws_byte_cursor *cur_alloc = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur_alloc != NULL);
        ensure_byte_cursor_has_allocated_buffer_member(cur_alloc);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur_alloc, MAX_BUFFER_SIZE));
        cur = cur_alloc;
    }

    /* Save old state of the cursor (if any) */
    const struct aws_byte_cursor *old_cur = cur;
    struct store_byte_from_buffer cur_store;
    if (cur != NULL) {
        save_byte_from_array(cur->ptr, cur->len, &cur_store);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Post‑condition: result matches the specification */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, cur->ptr, cur->len);
    }
    assert(result == expected);

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str == old_str);
        assert(str->allocator == old_str->allocator);
        assert(str->len == old_str->len);
        assert_bytes_match(str->bytes, old_str->bytes, str->len);
        assert_byte_from_buffer_matches(str->bytes, &str_store);
    }
    if (cur != NULL) {
        assert(cur == old_cur);
        assert(cur->ptr == old_cur->ptr);
        assert(cur->len == old_cur->len);
        assert_byte_from_buffer_matches(cur->ptr, &cur_store);
    }

    /* 6. Validity invariants */
    assert(aws_string_is_valid(str));
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
