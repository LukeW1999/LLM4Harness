#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *src = NULL;

    /* nondeterministically decide whether src is NULL */
    if (!nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate and nondeterministically fill a buffer */
        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }

        /* create the aws_string from the buffer */
        src = aws_string_new_from_array(allocator, buf, len);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));

        /* free the temporary buffer – the string makes its own copy */
        free(buf);
    }

    /* Save old state of src for later immutability checks */
    struct aws_string old_src;
    struct store_byte_from_buffer old_bytes;
    if (src != NULL) {
        old_src = *src;                     /* copies allocator, len, first byte */
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the function returns a zeroed cursor */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* When src is non‑NULL the cursor should reference the string's bytes */
        assert(cur.ptr == src->bytes);
        assert(cur.len == src->len);

        /* The source string must remain unchanged */
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* Validity invariants must hold after the call */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
    /* The returned cursor must be a well‑formed cursor */
    assert(aws_byte_cursor_is_bounded(&cur, MAX_STRING_LEN));
}
