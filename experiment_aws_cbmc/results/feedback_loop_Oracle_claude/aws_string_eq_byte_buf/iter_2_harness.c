#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 32
#endif

void aws_string_eq_byte_buf_harness(void) {
    /* Nondeterministic inputs */
    struct aws_string *str = ensure_string_is_allocated_bounded_length(MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    /* Ensure buf.buffer is non-NULL so memcmp precondition is satisfied when lengths match */
    __CPROVER_assume(buf.len == 0 || buf.buffer != NULL);

    /* Save state before call for frame condition checks */
    size_t old_str_len = str->len;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = buf.buffer;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postcondition 1: If result is true, lengths must match */
    if (result == true) {
        assert(str->len == buf.len);
    }

    /* Postcondition 2: If lengths differ, result must be false */
    if (str->len != buf.len) {
        assert(result == false);
    }

    /* Postcondition 3: Frame conditions - the function should not modify str or buf */
    assert(str->len == old_str_len);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.buffer == old_buf_buffer);

    /* Postcondition 4: Result is a valid boolean */
    assert(result == true || result == false);

    /* Postcondition 5: buf validity is preserved */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 6: str validity is preserved */
    assert(aws_string_is_valid(str));
}
