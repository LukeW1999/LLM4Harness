#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 8
#define MAX_BUF_LEN 8

bool aws_string_eq_byte_buf(const struct aws_string *str, const struct aws_byte_buf *buf);

void aws_string_eq_byte_buf_harness(void) {
    /* Set up str - always non-NULL to avoid NULL dereference issues */
    struct aws_string *str = make_arbitrary_aws_string_nondet_len_with_max(aws_default_allocator(), MAX_STRING_LEN);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Set up buf - always non-NULL */
    struct aws_byte_buf buf;
    size_t buf_len;
    __CPROVER_assume(buf_len <= MAX_BUF_LEN);
    uint8_t *buf_data = malloc(buf_len);
    __CPROVER_assume(buf_len == 0 || buf_data != NULL);
    buf.len = buf_len;
    buf.capacity = buf_len;
    buf.buffer = buf_data;
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state before call for frame condition checks */
    size_t old_str_len = str->len;
    size_t old_buf_len = buf.len;
    size_t old_buf_cap = buf.capacity;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postcondition: if lengths differ, result must be false */
    if (str->len != buf.len) {
        assert(result == false);
    }

    /* Postcondition: if result is true, lengths must be equal */
    if (result == true) {
        assert(str->len == buf.len);
    }

    /* Frame condition: str fields unchanged */
    assert(str->len == old_str_len);
    assert(aws_string_is_valid(str));

    /* Frame condition: buf fields unchanged */
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_cap);
    assert(aws_byte_buf_is_valid(&buf));
}
