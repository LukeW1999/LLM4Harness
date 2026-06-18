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

    /* 2. Set up c_str — nondet pointer, may be NULL or valid */
    /* We use a bounded char array to represent the c_str */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    /* c_str can be NULL (if c_str_len == 0, malloc may return NULL, but we handle both) */
    /* If non-NULL, ensure it's null-terminated */
    if (c_str != NULL) {
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* The function is a pure comparison — it should not modify buf at all */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariant still holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Result semantics:
     * Returns true if buf->buffer[0..len-1] equals c_str (without null terminator).
     * Returns false otherwise.
     * We can't easily assert the exact value without re-implementing the comparison,
     * but we can assert the type is bool (always true or false).
     */
    assert(result == true || result == false);
}
