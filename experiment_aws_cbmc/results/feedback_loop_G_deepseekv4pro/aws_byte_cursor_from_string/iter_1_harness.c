#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    /* Non-deterministically choose if src is NULL or a valid string */
    bool is_null = nondet_bool();
    const struct aws_string *src = NULL;
    struct aws_string *allocated_str = NULL;
    size_t len;
    struct store_byte_from_buffer old_byte;

    if (is_null) {
        src = NULL;
    } else {
        /* Allocate a valid aws_string with bounded length */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE); /* bound length to avoid state explosion */

        /* Allocate memory for the string header + len + 1 (null terminator).
         * sizeof(struct aws_string) already includes 1 byte for bytes[1],
         * so we need sizeof(struct aws_string) + len bytes total. */
        size_t total_size = sizeof(struct aws_string) + len;
        allocated_str = (struct aws_string *)malloc(total_size);
        if (allocated_str == NULL) {
            return; /* allocation failure, skip this test case */
        }

        /* Set fields */
        allocated_str->allocator = aws_default_allocator();
        allocated_str->len = len;

        /* Fill bytes with nondet data */
        for (size_t i = 0; i < len; i++) {
            allocated_str->bytes[i] = nondet_uint8_t();
        }
        allocated_str->bytes[len] = '\0'; /* null terminator */

        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(allocated_str));

        src = allocated_str;

        /* Save a byte from the string to check immutability */
        save_byte_from_array(src->bytes, src->len, &old_byte);
    }

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (is_null) {
        /* If src was NULL, cursor should be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* If src was non-NULL, cursor should point to string's bytes and have same length */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* Check that src is unchanged */
        assert(src->len == len);
        assert_byte_from_buffer_matches(src->bytes, &old_byte);

        /* Check that the string is still valid */
        assert(aws_string_is_valid(src));
    }

    /* Check that the returned cursor is valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Free allocated memory if any */
    if (allocated_str != NULL) {
        free(allocated_str);
    }
}
