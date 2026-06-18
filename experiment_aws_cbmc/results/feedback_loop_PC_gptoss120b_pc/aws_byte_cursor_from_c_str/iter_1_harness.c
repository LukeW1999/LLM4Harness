#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Declare nondeterministic input string */
    const char *c_str;
    bool is_null = nondet_bool();
    if (is_null) {
        c_str = NULL;
    } else {
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[len] = '\0';
        c_str = buf;
    }

    /* 2. Snapshot old state */
    const char *old_c_str = c_str;
    struct store_byte_from_buffer old_bytes;
    if (c_str != NULL) {
        size_t old_len = strlen(c_str);
        save_byte_from_array((const uint8_t *)c_str, old_len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Assert postconditions */
    /* Returned pointer must equal input pointer (cast) */
    assert(cur.ptr == (uint8_t *)c_str);

    /* Length must be strlen when input is non‑NULL, otherwise 0 */
    if (c_str != NULL) {
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.len == 0);
    }

    /* Input pointer must remain unchanged */
    assert(c_str == old_c_str);

    /* Input buffer contents must remain unchanged */
    if (c_str != NULL) {
        size_t old_len = strlen(c_str);
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_bytes);
    }

    /* 5. Invariant: returned cursor is valid */
    assert(aws_byte_cursor_is_valid(&cur));
}
