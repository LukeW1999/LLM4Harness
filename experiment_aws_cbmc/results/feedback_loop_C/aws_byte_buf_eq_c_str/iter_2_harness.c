#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    const char *c_str;

    // Initialize buf with arbitrary but valid values
    size_t max_size = nondet_size_t();
    __CPROVER_assume(max_size <= MAX_BUFFER_SIZE);
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(buf.capacity <= max_size);

    // Initialize c_str with a valid null-terminated string
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < str_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[str_len] = '\0';

    // Save old state
    struct aws_byte_buf old_buf = buf;

    // Call the function under test
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    // Assert frame conditions
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Assert validity invariants
    assert(aws_byte_buf_is_valid(&buf));

    // Assert postconditions
    if (result) {
        assert(buf.len == str_len);
        for (size_t i = 0; i < str_len; i++) {
            assert(buf.buffer[i] == c_str[i]);
        }
    } else {
        assert(!(buf.len == str_len && __CPROVER_forall { size_t i; (i < str_len) ==> (buf.buffer[i] == c_str[i]) }));
    }

    // Free allocated memory
    free(buf.buffer);
    free((void *)c_str);
}
