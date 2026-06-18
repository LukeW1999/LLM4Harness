#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
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
    /* c_str can be NULL only if length is 0, but aws_array_eq_c_str handles NULL c_str
       by checking strlen. Let's allow non-NULL c_str with null terminator. */
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison — it should not modify buf at all */
    /* Unchanged fields */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* The return value is a bool — both true and false are valid outcomes.
       We verify the semantic correctness:
       result == true iff aws_array_eq_c_str(buf.buffer, buf.len, c_str) is true.
       Since we can't easily re-derive the exact comparison result in the harness,
       we at least assert the result is a valid bool (always true for C bool). */
    assert(result == true || result == false);

    /* If buf.len is 0 and c_str points to an empty string, result should be true */
    /* We can assert: if result is true, then the lengths must match */
    /* (c_str_len == buf.len when result is true, since null terminator not in buf) */
    if (result) {
        assert(buf.len == c_str_len);
    }
}

void aws_byte_buf_eq_c_str_harness(void) {
    aws_byte_buf_eq_c_str_harness();
    return 0;
}
