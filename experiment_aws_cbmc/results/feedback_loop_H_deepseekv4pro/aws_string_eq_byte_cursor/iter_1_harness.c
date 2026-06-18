#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* Pointers to the two inputs */
    struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* Storage for saving original bytes */
    uint8_t old_str_bytes[MAX_BUFFER_SIZE];
    uint8_t old_cur_bytes[MAX_BUFFER_SIZE];

    /* Initialize string pointer */
    bool have_str = nondet_bool();
    size_t str_len = 0;
    if (have_str) {
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        /* Allocate contiguous memory for the struct aws_string header and its bytes */
        size_t total_size = sizeof(struct aws_string) + str_len + 1; /* +1 for null terminator */
        void *mem = malloc(total_size);
        if (mem == NULL) {
            return; /* Bail out if malloc fails (CBMC will skip this path) */
        }
        str = (struct aws_string *)mem;
        /* Initialize const fields by casting away const (permitted in harness setup) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = str_len;
        /* Fill the bytes array with nondeterministic values */
        for (size_t i = 0; i < str_len; i++) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[str_len] = 0; /* null terminator */
        /* Assume the constructed string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
        /* Save original bytes for immutability check */
        for (size_t i = 0; i < str_len; i++) {
            old_str_bytes[i] = str->bytes[i];
        }
    } else {
        str = NULL;
    }

    /* Initialize cursor */
    struct aws_byte_cursor cur_obj;
    bool have_cur = nondet_bool();
    size_t old_cur_len = 0;
    uint8_t *old_cur_ptr = NULL;
    if (have_cur) {
        cur = &cur_obj;
        cur_obj.len = nondet_size_t();
        __CPROVER_assume(cur_obj.len <= MAX_BUFFER_SIZE);
        ensure_byte_cursor_has_allocated_buffer_member(&cur_obj);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_obj));
        /* Fill cursor buffer with nondeterministic values */
        for (size_t i = 0; i < cur_obj.len; i++) {
            cur_obj.ptr[i] = nondet_uint8_t();
        }
        /* Save original cursor state for immutability check */
        old_cur_ptr = cur_obj.ptr;
        old_cur_len = cur_obj.len;
        for (size_t i = 0; i < cur_obj.len; i++) {
            old_cur_bytes[i] = cur_obj.ptr[i];
        }
    } else {
        cur = NULL;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* --- Postconditions: immutability --- */
    if (have_str) {
        /* The string struct and its bytes must not have changed */
        assert(str->allocator == aws_default_allocator());
        assert(str->len == str_len);
        for (size_t i = 0; i < str_len; i++) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
        assert(str->bytes[str_len] == 0);
        assert(aws_string_is_valid(str));
    }

    if (have_cur) {
        /* Cursor fields and buffer must remain unchanged */
        assert(cur->ptr == old_cur_ptr);
        assert(cur->len == old_cur_len);
        for (size_t i = 0; i < old_cur_len; i++) {
            assert(cur->ptr[i] == old_cur_bytes[i]);
        }
        assert(aws_byte_cursor_is_valid(cur));
    }

    /* --- Postcondition: return value correctness --- */
    bool expected;
    if (str == NULL && cur == NULL) {
        expected = true;
    } else if (str == NULL || cur == NULL) {
        expected = false;
    } else {
        if (str->len != cur->len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != cur->ptr[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);
}
