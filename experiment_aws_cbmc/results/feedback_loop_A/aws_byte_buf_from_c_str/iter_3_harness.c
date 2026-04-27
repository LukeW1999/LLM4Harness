#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    const char *c_str = (const char *)any_ptr();
    size_t str_len = strlen(c_str);
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Add assumptions to ensure c_str is a valid string */
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(__CPROVER_r_ok(c_str, str_len + 1));

    /* 3. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(buf.len == str_len);
    assert(buf.capacity == str_len + 1); // Include null terminator
    assert(buf.buffer != NULL);
    assert(buf.allocator == NULL);
    assert(memcmp(buf.buffer, c_str, str_len) == 0); // Check content instead of pointer
    assert(buf.buffer[str_len] == '\0'); // Check null terminator

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
