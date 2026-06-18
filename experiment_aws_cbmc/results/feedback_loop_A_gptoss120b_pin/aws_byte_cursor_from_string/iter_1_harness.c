#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer, nondeterministically NULL or a valid string */
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for struct + flexible array */
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        src->allocator = aws_default_allocator();
        src->len = len;

        /* initialize bytes (including null terminator) */
        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)src->bytes)[len] = 0; /* explicit null terminator */

        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) */
    struct aws_string old;
    if (src) {
        old = *src;
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* when src is NULL the cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* when src is non‑NULL the cursor must reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* src must remain unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        /* the bytes themselves must not be altered */
        for (size_t i = 0; i < src->len + 1; ++i) {
            assert(((uint8_t *)src->bytes)[i] == ((uint8_t *)old.bytes)[i]);
        }

        /* validity invariant must still hold */
        assert(aws_string_is_valid(src));
    }

    /* 5. The returned cursor should be bounded */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));

    /* clean up */
    free(src);
}
