#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* src may be NULL or a valid aws_string */
    struct aws_string *src;
    bool src_is_null = nondet_bool();

    if (src_is_null) {
        src = NULL;
    } else {
        size_t max_len = nondet_size_t();
        __CPROVER_assume(max_len <= MAX_BUFFER_SIZE);

        /* allocate enough space for the flexible array member */
        src = malloc(sizeof(struct aws_string) + max_len);
        __CPROVER_assume(src != NULL);

        src->allocator = aws_default_allocator();
        src->len = max_len;

        /* the bytes are nondeterministic; validity check will ensure they are acceptable */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Save old state of src when it is non‑NULL */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (src != NULL) {
        old = *src;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* Post‑conditions */
    if (src == NULL) {
        /* Empty cursor is returned */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* Cursor points to the string's bytes and has the same length */
        assert(cur.ptr == aws_string_bytes(src));
        assert(cur.len == src->len);

        /* src must remain unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);

        /* Validity invariant must still hold */
        assert(aws_string_is_valid(src));
    }
}
