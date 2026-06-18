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
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministic input string (may be NULL) */
    bool is_null = nondet_bool();
    const char *c_str = NULL;
    size_t max_len = MAX_BUFFER_SIZE;
    size_t str_len = 0;

    if (!is_null) {
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= max_len);
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* fill with nondet bytes */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)buf)[i] = nondet_uint8_t();
        }
        buf[str_len] = '\0';
        c_str = buf;
    }

    /* 2. Save old state of the input buffer (if any) */
    struct store_byte_from_buffer old_bytes;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, str_len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* 4. Post‑condition assertions */
    if (c_str == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        assert(result.ptr == (uint8_t *)c_str);
        /* strlen(c_str) == str_len because we placed the terminating NUL at that index */
        assert(result.len == str_len);
    }

    /* 5. Input buffer must remain unchanged */
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_bytes);
    }

    /* 6. Validity invariant for the returned cursor */
    assert(aws_byte_cursor_is_valid(&result));
}
