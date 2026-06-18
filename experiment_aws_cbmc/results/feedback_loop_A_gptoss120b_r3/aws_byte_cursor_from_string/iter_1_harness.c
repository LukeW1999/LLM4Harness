#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer (may be NULL) */
    struct aws_string *src = nondet_pointer();

    if (src) {
        /* Non-deterministic length bounded by MAX_BUFFER_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for struct + flexible array */
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        /* Initialize fields */
        src->allocator = NULL;               /* static string allowed */
        src->len = len;

        /* Bytes are nondeterministic */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }

        /* Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));

        /* 2. Save old state */
        struct aws_string old = *src;
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(src->bytes, src->len, &old_bytes);

        /* 3. Call function under test */
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        /* 4. Postconditions for non‑NULL src */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));

        /* 5. Unchanged fields of src */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);

        /* 6. Validity invariant */
        assert(aws_string_is_valid(src));
    } else {
        /* 3. Call function under test with NULL */
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(NULL);

        /* 4. Postconditions for NULL src */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    }
}
