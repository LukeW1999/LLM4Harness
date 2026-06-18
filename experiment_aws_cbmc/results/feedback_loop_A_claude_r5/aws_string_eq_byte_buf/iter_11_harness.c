#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void harness(void) {
    /* Set up str */
    struct aws_string *str = make_arbitrary_aws_string_nondet_len_with_max(MAX_STRING_LEN);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Set up buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member_with_max_size(&buf, MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    size_t old_str_len = str->len;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_buf_allocator = buf.allocator;
    uint8_t *old_buf_buffer = buf.buffer;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postconditions */
    /* If lengths differ, must return false */
    if (str->len != buf.len) {
        __CPROVER_assert(result == false, "lengths differ implies false");
    }
    /* If lengths are both 0, must return true */
    if (str->len == 0 && buf.len == 0) {
        __CPROVER_assert(result == true, "both empty implies true");
    }
    /* Immutability: str fields unchanged */
    __CPROVER_assert(str->len == old_str_len, "str->len unchanged");
    __CPROVER_assert(aws_string_is_valid(str), "str still valid");

    /* Immutability: buf fields unchanged */
    __CPROVER_assert(buf.len == old_buf_len, "buf.len unchanged");
    __CPROVER_assert(buf.capacity == old_buf_capacity, "buf.capacity unchanged");
    __CPROVER_assert(buf.allocator == old_buf_allocator, "buf.allocator unchanged");
    __CPROVER_assert(buf.buffer == old_buf_buffer, "buf.buffer unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf still valid");
}
