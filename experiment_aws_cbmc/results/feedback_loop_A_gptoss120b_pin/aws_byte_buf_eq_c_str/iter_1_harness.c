#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* 2. Allocate a nondeterministic null‑terminated C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a valid C string */
    c_str[c_len] = '\0';

    /* 3. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 4. Postconditions – the function is pure, so all fields must remain unchanged */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);

    /* 5. The result must equal the underlying array comparison */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* 6. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* clean up */
    free(c_str);
}
