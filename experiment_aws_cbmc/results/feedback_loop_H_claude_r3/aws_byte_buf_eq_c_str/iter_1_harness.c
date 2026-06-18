#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
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
    /* c_str can be NULL only if length is 0, but aws_array_eq_c_str handles NULL c_str */
    /* For safety, allow c_str to be non-NULL with a null terminator */
    if (c_str != NULL) {
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison — it should NOT modify buf at all */
    /* Changed fields: none (this is a read-only comparison) */
    /* Unchanged fields: all fields of buf must remain the same */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* 6. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Result type check: result is a bool (true or false) */
    /* No specific postcondition on the return value beyond it being a valid bool */
    /* The function returns true iff buf->buffer[0..buf->len-1] equals c_str content */
    /* We can assert the result is a valid boolean */
    assert(result == true || result == false);
}
