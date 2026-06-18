#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    // 1. Declare and initialize a valid byte buffer
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // 2. Save old state of the buffer and its contents
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_buffer_content;
    save_byte_from_array(buf.buffer, buf.len, &old_buffer_content);

    // 3. Prepare a nondeterministic, null-terminated C string of bounded size
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < str_len; i++) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[str_len] = '\0';

    // 4. Call the function under test
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    // 5. Postconditions – buffer remains completely unchanged
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
    if (buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_buffer_content);
    }
    // Buffer validity invariant must hold
    assert(aws_byte_buf_is_valid(&buf));

    // 6. Free allocated memory (cleanup for verification)
    free(c_str);
    free(buf.buffer);
}
