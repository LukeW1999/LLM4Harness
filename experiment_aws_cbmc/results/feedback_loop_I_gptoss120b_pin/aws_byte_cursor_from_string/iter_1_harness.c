#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer, possibly NULL */
    const struct aws_string *src;
    bool src_is_null = nondet_bool();

    if (src_is_null) {
        src = NULL;
    } else {
        /* allocate a non‑deterministic length for the string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate memory for struct + flexible array */
        struct aws_string *tmp = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(tmp != NULL);

        /* initialize fields */
        tmp->allocator = aws_default_allocator();
        /* const field, but we can assign via cast for harness purposes */
        *((size_t *)&tmp->len) = len;   /* bypass constness for initialization */

        /* fill the bytes with nondet data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)tmp->bytes)[i] = nondet_uint8_t();
        }

        /* ensure the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(tmp));

        src = tmp;
    }

    /* 2. Save old state of src (if non‑NULL) */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes = {0};
    if (src != NULL) {
        old = *src; /* copy allocator and len (bytes array not copied) */
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Postconditions */
    if (src == NULL) {
        /* when src is NULL, cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* when src is non‑NULL, cursor should reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        /* cursor must be bounded */
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    }

    /* 5. Unchanged fields of src (if non‑NULL) */
    if (src != NULL) {
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        /* bytes must be unchanged */
        assert_bytes_match(src->bytes, old_bytes.buffer, src->len);
    }

    /* 6. Validity invariants */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
