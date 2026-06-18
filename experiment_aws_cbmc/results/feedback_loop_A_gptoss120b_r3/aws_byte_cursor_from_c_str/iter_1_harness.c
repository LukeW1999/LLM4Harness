#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministic input string */
    const char *c_str;
    const char *old_c_str;
    size_t len = 0;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        /* bound the length of the allocated string */
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        /* allocate len+1 bytes for the string and the terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        /* fill the string with nondeterministic bytes (non‑NUL) */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)c_str)[i] = nondet_uint8_t();
        }
        /* ensure NUL termination */
        ((uint8_t *)c_str)[len] = '\0';
    }

    old_c_str = c_str;

    /* 2. Save old memory state for immutability check */
    struct store_byte_from_buffer storage;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, len + 1, &storage);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Post‑condition checks */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == len);
    }

    /* 5. Unchanged input pointer */
    assert(c_str == old_c_str);

    /* 6. Input memory must remain unchanged */
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &storage);
    }

    /* 7. Validity invariant for the returned cursor */
    assert(aws_byte_cursor_is_valid(&cur));
}
