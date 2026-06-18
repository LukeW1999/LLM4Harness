#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    char c_str_storage[MAX_BUFFER_SIZE + 1];
    c_str_storage[MAX_BUFFER_SIZE] = '\0';

    const char *c_str = nondet_bool() ? NULL : c_str_storage;

    struct store_byte_from_buffer old_byte;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str_storage, MAX_BUFFER_SIZE + 1, &old_byte);
    }

    size_t old_len = c_str ? strlen(c_str) : 0;

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == old_len);
        assert(cur.len == strlen(c_str));
        assert_byte_from_buffer_matches((const uint8_t *)c_str_storage, &old_byte);
    }

    assert(aws_byte_cursor_is_valid(&cur));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
