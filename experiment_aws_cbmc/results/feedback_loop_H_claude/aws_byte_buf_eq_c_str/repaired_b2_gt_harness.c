#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Set up c_str (non-deterministic null-terminated string) */
    const char *c_str = ensure_c_str_is_allocated(MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* 3. Save old state */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.len, &old_byte);
    size_t str_len = strlen(c_str);
    struct store_byte_from_buffer old_byte_from_str;
    save_byte_from_array((uint8_t *)c_str, str_len, &old_byte_from_str);

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */
    if (result) {
        assert(buf.len == str_len);
        if (buf.len > 0) {
            assert_bytes_match(buf.buffer, (uint8_t *)c_str, buf.len);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
    assert_byte_buf_equivalence(&buf, &old, &old_byte);
    if (str_len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_byte_from_str);
    }
}
