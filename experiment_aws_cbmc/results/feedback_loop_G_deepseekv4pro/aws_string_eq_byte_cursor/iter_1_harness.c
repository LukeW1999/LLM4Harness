#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    /* ----- 1. Declare and non-deterministically initialise the inputs ----- */
    struct aws_string *str_ptr = NULL;
    struct aws_byte_cursor *cur_ptr = NULL;

    /* String: either NULL or a valid aws_string */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        size_t extra = (len > 0) ? (len - 1) : 0;
        size_t alloc_size = sizeof(struct aws_string) + extra;
        str_ptr = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str_ptr != NULL);  /* allocation succeeds */

        /* Write fields and bytes (casting away const is fine in CBMC) */
        *(struct aws_allocator **)&str_ptr->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        *(size_t *)&str_ptr->len = len;
        for (size_t i = 0; i < len; i++) {
            *(uint8_t *)&str_ptr->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str_ptr));
    } else {
        str_ptr = NULL;
    }

    /* Cursor: either NULL or a valid aws_byte_cursor (allocated on stack) */
    struct aws_byte_cursor cur_mem;
    if (nondet_bool()) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_mem, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_mem);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_mem));
        cur_ptr = &cur_mem;
    } else {
        cur_ptr = NULL;
    }

    /* ----- 2. Save old states for immutability checks ----- */
    uint8_t old_str_bytes[MAX_BUFFER_SIZE];
    size_t old_str_len = 0;
    struct aws_allocator * old_str_alloc = NULL;
    if (str_ptr != NULL) {
        old_str_len = str_ptr->len;
        old_str_alloc = str_ptr->allocator;
        for (size_t i = 0; i < old_str_len; i++) {
            old_str_bytes[i] = str_ptr->bytes[i];
        }
    }

    uint8_t old_cur_bytes[MAX_BUFFER_SIZE];
    size_t old_cur_len = 0;
    const uint8_t * old_cur_ptr = NULL;
    if (cur_ptr != NULL) {
        old_cur_len = cur_ptr->len;
        old_cur_ptr = cur_ptr->ptr;
        for (size_t i = 0; i < old_cur_len; i++) {
            old_cur_bytes[i] = cur_ptr->ptr[i];
        }
    }

    /* ----- 3. Call the function under test ----- */
    bool result = aws_string_eq_byte_cursor(str_ptr, cur_ptr);

    /* ----- 4. Postconditions ------------------------------------------------- */

    /* 4a. Return value correctness according to the specification */
    bool expected;
    if (str_ptr == NULL && cur_ptr == NULL) {
        expected = true;
    } else if (str_ptr == NULL || cur_ptr == NULL) {
        expected = false;
    } else {
        /* Both non‑NULL: equality of lengths and content */
        if (str_ptr->len == cur_ptr->len) {
            expected = (memcmp(str_ptr->bytes, cur_ptr->ptr, str_ptr->len) == 0);
        } else {
            expected = false;
        }
    }
    assert(result == expected);

    /* 4b. String remains unchanged (when non‑NULL) */
    if (str_ptr != NULL) {
        assert(str_ptr->len == old_str_len);
        assert(str_ptr->allocator == old_str_alloc);
        for (size_t i = 0; i < old_str_len; i++) {
            assert(str_ptr->bytes[i] == old_str_bytes[i]);
        }
    } else {
        assert(str_ptr == NULL);
    }

    /* 4c. Cursor remains unchanged (when non‑NULL) */
    if (cur_ptr != NULL) {
        assert(cur_ptr->len == old_cur_len);
        assert(cur_ptr->ptr == old_cur_ptr);
        for (size_t i = 0; i < old_cur_len; i++) {
            assert(cur_ptr->ptr[i] == old_cur_bytes[i]);
        }
    } else {
        assert(cur_ptr == NULL);
    }

    /* 4d. Validity invariants hold after the call */
    if (str_ptr != NULL) {
        assert(aws_string_is_valid(str_ptr));
    }
    if (cur_ptr != NULL) {
        assert(aws_byte_cursor_is_valid(cur_ptr));
    }

    /* ----- 5. Cleanup (optional, avoids memory leak warnings) ----- */
    if (str_ptr != NULL) {
        free(str_ptr);
    }
}
