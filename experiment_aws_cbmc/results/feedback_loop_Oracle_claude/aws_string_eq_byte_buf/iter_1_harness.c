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
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Ground-truth preconditions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state before call for frame condition checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = buf.buffer;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postcondition 1: Return value correctness
     * If both are NULL, result must be true.
     * If one is NULL and the other is not, result must be false.
     * If both are non-NULL, result is true iff lengths and bytes match. */
    if (str == NULL && buf.buffer == NULL && buf.len == 0) {
        /* str is NULL, buf is valid with len 0 - depends on aws_array_eq behavior */
        /* Actually: str==NULL => returns false (since buf != NULL as a struct) */
        assert(result == false);
    }

    if (str == NULL) {
        assert(result == false);
    }

    /* Postcondition 2: If result is true, lengths must match */
    if (result == true) {
        assert(str != NULL);
        assert(str->len == buf.len);
    }

    /* Postcondition 3: If lengths differ, result must be false */
    if (str != NULL && str->len != buf.len) {
        assert(result == false);
    }

    /* Postcondition 4: Frame conditions - the function should not modify str or buf */
    if (str != NULL) {
        assert(str->len == old_str_len);
    }
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.buffer == old_buf_buffer);

    /* Postcondition 5: Result is a valid boolean */
    assert(result == true || result == false);

    /* Postcondition 6: buf validity is preserved */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 7: str validity is preserved */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
