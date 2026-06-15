#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    const struct aws_string *src;

    /* nondeterministically decide whether src is NULL */
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* bound the length of the string */
        const size_t MAX_STRING_LEN = 256;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for the struct plus the flexible array */
        struct aws_string *tmp = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        if (tmp) {
            tmp->allocator = NULL;               /* allocator is not relevant for this harness */
            tmp->len = len;

            /* initialise the bytes */
            uint8_t *bytes = (uint8_t *)tmp->bytes;
            for (size_t i = 0; i < len; ++i) {
                bytes[i] = nondet_uint8_t();
            }

            src = tmp;
            __CPROVER_assume(aws_string_is_valid(src));
        } else {
            src = NULL;
        }
    }

    /* save old state of src (if non‑NULL) for immutability checks */
    struct aws_string old;
    uint8_t *old_bytes = NULL;
    if (src) {
        old.allocator = src->allocator;
        old.len = src->len;

        if (src->len > 0) {
            old_bytes = malloc(src->len);
            if (old_bytes) {
                for (size_t i = 0; i < src->len; ++i) {
                    old_bytes[i] = src->bytes[i];
                }
            }
        }
    }

    /* call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* post‑conditions */
    if (src == NULL) {
        /* when src is NULL the cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* cursor must reference the string's bytes and length */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* src must be unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        if (old_bytes) {
            assert_bytes_match(src->bytes, old_bytes, src->len);
        }

        /* validity invariants */
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_bounded(&cursor, src->len));
    }
}
