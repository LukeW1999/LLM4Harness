#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    char backing[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        backing[i] = (char)nondet_uint8_t();
    }

    for (size_t i = 0; i < c_str_len; ++i) {
        __CPROVER_assume(backing[i] != '\0');
    }
    backing[c_str_len] = '\0';

    const char *c_str = nondet_bool() ? NULL : backing;

    if (c_str != NULL) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    }

    const char *old_c_str = c_str;

    struct store_byte_from_buffer old_backing_byte;
    save_byte_from_array((const uint8_t *)backing, c_str_len + 1, &old_backing_byte);

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    assert(c_str == old_c_str);
    assert_byte_from_buffer_matches((const uint8_t *)backing, &old_backing_byte);

    assert(cur.ptr == (uint8_t *)old_c_str);

    if (old_c_str == NULL) {
        assert(cur.len == 0);
        assert(cur.ptr == NULL);
    } else {
        assert(cur.len == c_str_len);
        assert(cur.ptr == (uint8_t *)backing);
    }

    assert(cur.len <= MAX_BUFFER_SIZE);
    assert(aws_byte_cursor_is_valid(&cur));
}
