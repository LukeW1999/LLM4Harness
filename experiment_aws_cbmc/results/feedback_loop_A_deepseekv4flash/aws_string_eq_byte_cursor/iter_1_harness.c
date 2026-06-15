#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 256
#endif

void aws_string_eq_byte_cursor_harness() {
    /* Non-deterministic inputs */
    const struct aws_string *str = NULL;
    const struct aws_byte_cursor *cur = NULL;

    /* Decide if str is NULL */
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* Allocate memory for the string header plus bytes */
        struct aws_string_mutable {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[1];
        };
        size_t total_size = sizeof(struct aws_string_mutable) + len;
        struct aws_string_mutable *raw_str = malloc(total_size);
        __CPROVER_assume(raw_str != NULL);
        raw_str->allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));
        raw_str->len = len;
        for (size_t i = 0; i < len; i++) {
            raw_str->bytes[i] = nondet_uint8_t();
        }
        str = (const struct aws_string *)raw_str;
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len));
    }

    /* Decide if cur is NULL */
    bool cur_is_null = nondet_bool();
    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        ensure_byte_cursor_has_allocated_buffer_member((struct aws_byte_cursor *)cur);
        size_t cur_len = nondet_size_t();
        __CPROVER_assume(cur_len <= MAX_BUFFER_SIZE);
        ((struct aws_byte_cursor *)cur)->len = cur_len;
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
        __CPROVER_assume(AWS_MEM_IS_READABLE(cur->ptr, cur->len));
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    struct aws_byte_cursor old_cur;
    if (str != NULL) {
        old_str = *str;
    }
    if (cur != NULL) {
        old_cur = *cur;
    }

    /* Call function */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions */
    /* 1. Result correctness */
    if (str != NULL && cur != NULL) {
        bool expected = (str->len == cur->len) && (memcmp(str->bytes, cur->ptr, str->len) == 0);
        assert(result == expected);
    } else if (str == NULL && cur == NULL) {
        assert(result == true);
    } else {
        assert(result == false);
    }

    /* 2. Immutability: structures unchanged */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(AWS_MEM_IS_READABLE(str->bytes, str->len));
    }
    if (cur != NULL) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
        assert(AWS_MEM_IS_READABLE(cur->ptr, cur->len));
    }

    /* 3. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
