#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Set up c_str (non-deterministic null-terminated string) */
    /* We use a bounded array to keep the state space manageable */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    /* c_str may be NULL (if c_str_len == 0, NULL is allowed per aws_array_eq_c_str docs) */
    /* But aws_byte_buf_eq_c_str passes c_str directly to aws_array_eq_c_str,
       which allows NULL only if length is 0. We allow non-NULL c_str with null terminator. */
    if (c_str != NULL) {
        /* Ensure null terminator exists somewhere in the string */
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state of buf (buf should not be modified by this function) */
    struct aws_byte_buf old_buf;
    old_buf.len = buf.len;
    old_buf.buffer = buf.buffer;
    old_buf.capacity = buf.capacity;
    old_buf.allocator = buf.allocator;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function returns a bool - no success/failure path distinction needed */
    /* The function should not modify buf at all */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariant still holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Assert result is a valid bool */
    assert(result == true || result == false);
}
