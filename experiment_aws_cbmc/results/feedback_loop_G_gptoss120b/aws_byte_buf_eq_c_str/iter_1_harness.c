#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Create a nondeterministic C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a proper null‑terminated string */
    c_str[c_str_len] = '\0';

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert that no fields of the buffer changed */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);

    /* 6. The function returns the result of aws_array_eq_c_str */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* 7. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));
}
