#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state of the buffer */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_storage;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_buf_storage);
    }

    /* 2. Declare and bound the C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    if (c_str) {
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    /* Save old state of the C string */
    char *old_c_str = c_str;
    struct store_byte_from_buffer old_c_str_storage;
    if (c_str) {
        save_byte_from_array((const uint8_t *)c_str, c_len + 1, &old_c_str_storage);
    }

    /* 3. Compute the expected result using the specification helper */
    bool expected = false;
    if (c_str) {
        expected = aws_array_eq_c_str(buf.buffer, buf.len, c_str);
    }

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert return value matches specification */
    assert(result == expected);

    /* 6. Assert unchanged fields of the byte buffer */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    if (buf.buffer != NULL && buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_buf_storage);
    }

    /* 7. Assert unchanged C‑string */
    assert(c_str == old_c_str);
    if (c_str) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_storage);
    }

    /* 8. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));

    /* clean‑up */
    free(c_str);
}
