#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Declare and allocate a null‑terminated C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 3. Save old state of the buffer */
    struct aws_byte_buf old_buf = buf;

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert that no fields of the buffer have changed */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. The result must equal the underlying array comparison */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* 7. The buffer must remain valid after the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    free(c_str);
}
