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

    /* 2. Set up c_str — a non-deterministic null-terminated string */
    /* We use a bounded array to keep state space manageable */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator exists */
    c_str[c_str_len] = '\0';

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

    /* 7. Verify result semantics:
     * aws_byte_buf_eq_c_str returns true iff buf->buffer[0..len-1] equals c_str[0..strlen(c_str)-1]
     * This is delegated to aws_array_eq_c_str which compares buf->buffer with c_str.
     * We can assert that if result is true, then the lengths must match
     * (the array should NOT contain a null-terminator, or comparison always returns false).
     */
    if (result) {
        /* If they are equal, the c_str length must equal buf.len */
        size_t actual_c_str_len = 0;
        const char *p = c_str;
        while (*p != '\0') {
            actual_c_str_len++;
            p++;
        }
        assert(actual_c_str_len == buf.len);
    }
}
