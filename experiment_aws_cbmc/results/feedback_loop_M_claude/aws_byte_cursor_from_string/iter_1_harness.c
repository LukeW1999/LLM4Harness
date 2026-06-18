#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_byte_cursor_from_string
 *
 * From Doxygen:
 *   "Creates an aws_byte_cursor from an existing string.
 *    If the src is NULL, it returns an empty cursor"
 *
 * Analysis:
 * 1. Changed fields: returns a new aws_byte_cursor with ptr and len set from the string
 * 2. Unchanged fields: src string is not modified
 * 3. Failure path: if src is NULL, returns empty cursor (ptr=NULL, len=0)
 * 4. Validity invariants: returned cursor must be valid
 */

/* Allocate a valid aws_string with bounded length */
struct aws_string *ensure_string_is_allocated(size_t len) {
    /* Allocate memory for the string header + data bytes + null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    /* Set the fields - need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    /* bytes are nondet (already nondet from malloc) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or a valid string */
    bool is_null = nondet_bool();

    struct aws_string *src = NULL;

    if (!is_null) {
        /* Create a bounded string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        src = ensure_string_is_allocated(len);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, cursor points to string bytes with same length */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        /* The string itself is unchanged */
        assert(aws_string_is_valid(src));
    }

    /* Validity invariant: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}

void aws_byte_cursor_from_string_harness(void) {
    aws_byte_cursor_from_string_harness();
    return 0;
}
