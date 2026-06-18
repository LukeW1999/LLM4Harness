#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Create a nondeterministic C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_len] = '\0';

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert unchanged fields (function is pure) */
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.len == old.len);

    /* 6. Assert validity invariant holds after the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Consistency with the underlying array comparison */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* Clean up */
    free(c_str);
}
