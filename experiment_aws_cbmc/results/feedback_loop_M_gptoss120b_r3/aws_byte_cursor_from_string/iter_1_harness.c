#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Non‑deterministic length for a possible string, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a buffer for the string bytes (may be NULL) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        /* If allocation succeeded, the buffer is readable for `len` bytes */
        if (bytes) {
            __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
        }
    }

    /* 3. Create a possibly NULL aws_string using the library constructor */
    struct aws_string *src = aws_string_new_from_array(aws_default_allocator(), bytes, len);
    /* 4. Assume the string (if non‑NULL) satisfies the library validity predicate */
    __CPROVER_assume(src == NULL || aws_string_is_valid(src));

    /* 5. Save old state for immutability checks */
    const struct aws_string *old_src = src;

    /* 6. Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 7. Post‑condition: behavior when src is NULL */
    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* 8. Post‑condition: behavior when src is non‑NULL */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 9. Unchanged fields: the source pointer itself must not be modified */
    assert(src == old_src);

    /* 10. Validity invariants after the call */
    assert(src == NULL || aws_string_is_valid(src));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
