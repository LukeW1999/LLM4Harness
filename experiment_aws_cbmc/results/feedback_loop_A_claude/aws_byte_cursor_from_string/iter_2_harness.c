#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or a valid string */
    struct aws_string *src;
    
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* Use the proof helper to create a valid aws_string */
        src = ensure_string_is_allocated_bounded_length(MAX_BUFFER_SIZE);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, result should be an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, result should point to src->bytes with src->len */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);

        /* The input string must not have been modified */
        assert(aws_string_is_valid(src));

        /* The returned cursor should be valid */
        assert(aws_byte_cursor_is_valid(&result));
    }
}
