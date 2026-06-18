#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or a valid string */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    struct aws_string *src = NULL;

    if (!is_null) {
        /* Create a bounded string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string header + data bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        src = malloc(total_size);
        __CPROVER_assume(src != NULL);
        /* Set the fields - need to cast away const for initialization */
        *(size_t *)&src->len = len;
        /* Ensure null terminator */
        ((uint8_t *)src->bytes)[len] = 0;
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
