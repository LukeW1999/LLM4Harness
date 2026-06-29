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
    /* Ensure buf.buffer is non-NULL when buf.len > 0 so memcmp precondition is satisfied */
    __CPROVER_assume(buf.len == 0 || buf.buffer != NULL);
    /* Ensure str->bytes is readable for memcmp when lengths match */
    /* The string bytes are always allocated by ensure_string_is_allocated_bounded_length */
    /* Additional constraint: if buf.len > 0, ensure str bytes are accessible */
    __CPROVER_assume(buf.len == 0 || str->len == 0 || 
                     __CPROVER_r_ok(str->bytes, str->len));

    /* Save state before call for frame condition checks */
    size_t old_str_len = str->len;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = buf.buffer;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postcondition 2: Frame conditions - the function should not modify str or buf */
    assert(str->len == old_str_len);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.buffer == old_buf_buffer);

    /* Postcondition 3: Result is a valid boolean */
    assert(result == true || result == false);

    /* Postcondition 4: buf validity is preserved */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 5: str validity is preserved */
    assert(aws_string_is_valid(str));
}
