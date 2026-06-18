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
 * 1. Changed fields: returns a new aws_byte_cursor (ptr and len set from string)
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

    /* The bytes array is already allocated as part of the struct */
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or valid */
    bool is_null = nondet_bool();

    struct aws_string *src = NULL;

    if (!is_null) {
        /* Create a valid aws_string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for struct + bytes + null terminator */
        src = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(src != NULL);

        /* Initialize fields */
        *(struct aws_allocator **)&src->allocator = aws_default_allocator();
        *(size_t *)&src->len = len;

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Save old state of src (if non-null) */
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes = (src != NULL) ? src->bytes : NULL;
    struct aws_allocator *old_allocator = (src != NULL) ? src->allocator : NULL;

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Assert postconditions */
    if (src == NULL) {
        /* If src is NULL, returns empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is non-NULL, cursor points to string bytes with same length */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);

        /* src string must not be modified */
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);
        assert(src->allocator == old_allocator);

        /* Returned cursor must be valid */
        assert(aws_byte_cursor_is_valid(&result));
    }
}
