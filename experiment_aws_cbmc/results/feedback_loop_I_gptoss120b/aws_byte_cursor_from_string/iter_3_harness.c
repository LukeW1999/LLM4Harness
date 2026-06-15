#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src;

    /* Choose between NULL and a valid string */
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate struct plus flexible array (len bytes + NUL) */
        src = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(src != NULL);

        src->allocator = aws_default_allocator();
        src->len = len;

        uint8_t *bytes = (uint8_t *)src->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0; /* NUL‑terminator, not counted in len */

        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Preserve old state for frame‑condition checks */
    struct aws_string old;
    struct store_byte_from_buffer old_bytes;
    if (src != NULL) {
        old = *src;                     /* copy scalar fields */
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* Post‑condition assertions */
    if (src == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == aws_string_bytes(src));
        assert(cur.len == src->len);

        /* Frame condition: src unchanged */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* Validity invariants */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
