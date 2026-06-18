#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_string_eq_byte_cursor
 *
 * From the implementation:
 *   - If both str and cur are NULL, returns true
 *   - If exactly one is NULL, returns false
 *   - Otherwise, returns aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)
 *
 * The function is a pure predicate: it does not modify any of its inputs.
 *
 * Postconditions:
 * 1. RETURN: result is true iff both NULL, or bytes match; false if exactly one NULL
 * 2. FRAME: str and cur (and their contents) are unchanged
 * 3. INVARIANTS: str and cur remain valid after the call
 */

/* Maximum string/cursor size to keep state space manageable */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 4
#endif

void aws_string_eq_byte_cursor_harness(void) {
    /* ------------------------------------------------------------------ */
    /* 1. Set up aws_string (may be NULL) */
    /* ------------------------------------------------------------------ */
    const struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    /* We need a concrete aws_string with bounded length */
    struct aws_string *str_obj = NULL;

    if (!str_is_null) {
        /* Allocate a concrete aws_string with a bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* aws_string layout: header + bytes[len] + null terminator */
        size_t alloc_size = sizeof(struct aws_string) + str_len + 1;
        str_obj = malloc(alloc_size);
        __CPROVER_assume(str_obj != NULL);

        /* Initialize the length field (const, so use a cast) */
        *((size_t *)&str_obj->len) = str_len;

        /* bytes are non-deterministic (already non-det from malloc) */
        /* Ensure null terminator */
        ((uint8_t *)str_obj->bytes)[str_len] = '\0';

        /* allocator can be anything (including NULL for static strings) */
        str = str_obj;
    }

    /* ------------------------------------------------------------------ */
    /* 2. Set up aws_byte_cursor (may be NULL) */
    /* ------------------------------------------------------------------ */
    struct aws_byte_cursor *cur = NULL;
    bool cur_is_null = nondet_bool();

    struct aws_byte_cursor cur_obj;

    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_obj, MAX_STRING_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_obj);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_obj));
        cur = &cur_obj;
    }

    /* ------------------------------------------------------------------ */
    /* 3. Snapshot inputs before the call (FRAME conditions) */
    /* ------------------------------------------------------------------ */
    /* Snapshot str fields */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    const uint8_t *old_str_bytes_ptr = (str != NULL) ? str->bytes : NULL;

    /* Snapshot cur fields */
    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* Save a byte from str->bytes for immutability check */
    struct store_byte_from_buffer str_byte_storage;
    if (str != NULL && str->len > 0) {
        save_byte_from_array(str->bytes, str->len, &str_byte_storage);
    }

    /* Save a byte from cur->ptr for immutability check */
    struct store_byte_from_buffer cur_byte_storage;
    if (cur != NULL && cur->len > 0) {
        save_byte_from_array(cur->ptr, cur->len, &cur_byte_storage);
    }

    /* ------------------------------------------------------------------ */
    /* 4. Call the function under test */
    /* ------------------------------------------------------------------ */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* ------------------------------------------------------------------ */
    /* 5. Assert postconditions */
    /* ------------------------------------------------------------------ */

    /* --- RETURN value postconditions --- */

    /* Case: both NULL → must return true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* Case: exactly one NULL → must return false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* Case: neither NULL → result depends on content (we can't assert the
     * exact value without re-implementing aws_array_eq, but we can assert
     * that if lengths differ the result must be false) */
    if (str != NULL && cur != NULL) {
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are equal and both zero, bytes trivially match */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
    }

    /* --- FRAME: str is unchanged --- */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes_ptr);
        /* Byte contents unchanged */
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &str_byte_storage);
        }
    }

    /* --- FRAME: cur is unchanged --- */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
        /* Byte contents unchanged */
        if (cur->len > 0) {
            assert_byte_from_buffer_matches(cur->ptr, &cur_byte_storage);
        }
    }

    /* --- INVARIANTS: validity still holds --- */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
