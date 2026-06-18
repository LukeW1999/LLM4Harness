#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Non‑deterministically decide whether src is NULL */
    struct aws_string *src = NULL;
    if (nondet_bool()) {
        /* src remains NULL */
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* prepare a buffer with nondet contents and a null terminator */
        uint8_t buf[MAX_STRING_LEN + 1];
        for (size_t i = 0; i < len + 1; ++i) {
            buf[i] = nondet_uint8_t();
        }
        buf[len] = 0; /* explicit null terminator */

        src = aws_string_new_from_array(aws_default_allocator(), buf, len);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) */
    size_t old_len = 0;
    uint8_t old_bytes[MAX_STRING_LEN + 1];
    if (src) {
        old_len = src->len;
        for (size_t i = 0; i < src->len + 1; ++i) {
            old_bytes[i] = ((uint8_t *)src->bytes)[i];
        }
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
        assert(src->len == old_len);
        for (size_t i = 0; i < src->len + 1; ++i) {
            assert(((uint8_t *)src->bytes)[i] == old_bytes[i]);
        }

        /* validity invariant must still hold */
        assert(aws_string_is_valid(src));
    }

    /* 5. The returned cursor should be bounded */
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));

    /* clean up */
    aws_string_destroy(src);
}
