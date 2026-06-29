#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_eq_c_str_harness(void) {
    struct aws_byte_buf buf;
    char *c_str;

    /* Setup: use ground-truth preconditions exactly */
    c_str = nondet_char_ptr();
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state before call for frame condition checks */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postcondition 1: Return value is a valid boolean (true or false) */
    assert(result == true || result == false);

    /* Postcondition 2: Buffer length invariant - len must not change */
    assert(buf.len == old_len);

    /* Postcondition 3: Buffer capacity invariant - capacity must not change */
    assert(buf.capacity == old_capacity);

    /* Postcondition 4: Buffer pointer must not change */
    assert(buf.buffer == old_buffer);

    /* Postcondition 5: Allocator must not change */
    assert(buf.allocator == old_allocator);

    /* Postcondition 6: The buffer must still be valid after the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 7: If result is true, then buf.len must equal strlen(c_str) */
    if (result) {
        assert(buf.len == strlen(c_str));
    }

    /* Postcondition 8: If buf.len != strlen(c_str), result must be false */
    if (buf.len != strlen(c_str)) {
        assert(!result);
    }
}
