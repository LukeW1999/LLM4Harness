#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Allocate and initialize a nondeterministic C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    /* If allocation fails, treat as NULL (the function under test can handle NULL) */
    if (c_str) {
        uint8_t *bytes = (uint8_t *)c_str;
        for (size_t i = 0; i < c_str_len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state for immutability checks */
    struct aws_byte_buf old_buf = buf;
    char *old_c_str = c_str;

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert that all fields of the buffer remain unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert that the C‑string pointer itself is unchanged */
    assert(c_str == old_c_str);

    /* 7. The buffer must still be valid after the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* 8. Clean up */
    free(c_str);
}
