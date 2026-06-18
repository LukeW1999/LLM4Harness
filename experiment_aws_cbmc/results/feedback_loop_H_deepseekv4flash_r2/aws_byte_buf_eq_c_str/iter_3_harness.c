#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf buf;
    
    /* c_str must be non-null and null-terminated */
    size_t c_str_len;
    __CPROVER_assume(c_str_len >= 0 && c_str_len < MAX_STRING_LEN);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    if (c_str_len > 0) {
        for (size_t i = 0; i < c_str_len; i++) {
            c_str[i] = (char)nondet_byte();
        }
    }
    c_str[c_str_len] = '\0'; // Null-terminate

    /* Assume buffer is bounded and valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call function */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postconditions */
    /* 1. Changed fields: none (function is read-only) */
    /* 2. Unchanged fields */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    /* 3. Both return paths: result is boolean, no failure path */
    /* 4. Validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
