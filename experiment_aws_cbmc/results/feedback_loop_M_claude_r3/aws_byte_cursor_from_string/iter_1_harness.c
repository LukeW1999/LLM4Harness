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
 * 1. Changed fields: returns a new aws_byte_cursor with ptr and len set
 * 2. Unchanged fields: src string is not modified
 * 3. Failure path: if src is NULL, returns empty cursor (ptr=NULL, len=0)
 * 4. Validity invariants: returned cursor must be valid
 */

/* Helper to allocate a valid aws_string with bounded length */
struct aws_string *ensure_string_is_allocated_bounded(size_t max_len) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* Allocate enough memory for the struct + len bytes + 1 null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize the fields - need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;

    /* The bytes are already allocated as part of the struct */
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or a valid string */
    bool is_null = nondet_bool();

    const struct aws_string *src;

    if (is_null) {
        src = NULL;
    } else {
        /* Create a valid aws_string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize fields */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        src = str;
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, cursor points to string bytes with correct length */
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));

        /* If len > 0, ptr must be non-null */
        if (src->len > 0) {
            assert(result.ptr != NULL);
        }
    }

    /* Validity invariant: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));

    /* If src was non-null, it must remain unchanged */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
