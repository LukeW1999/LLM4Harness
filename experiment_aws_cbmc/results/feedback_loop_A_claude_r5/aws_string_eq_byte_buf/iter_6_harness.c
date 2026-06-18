#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void harness(void) {
    /* Set up str */
    const struct aws_string *str = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);

    /* Set up buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member_with_max_size(&buf, MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    size_t old_str_len = str ? str->len : 0;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postconditions */
    if (str != NULL) {
        /* If lengths differ, must return false */
        if (str->len != buf.len) {
            assert(result == false);
        }
        /* If lengths are both 0, must return true */
        if (str->len == 0 && buf.len == 0) {
            assert(result == true);
        }
        /* Immutability: str fields unchanged */
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    /* Immutability: buf fields unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(aws_byte_buf_is_valid(&buf));
}
