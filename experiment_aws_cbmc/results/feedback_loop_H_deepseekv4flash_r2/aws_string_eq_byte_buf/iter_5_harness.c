#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string str;
    struct aws_byte_buf buf;

    /* Non-deterministic initialization of string */
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);
    str.allocator = aws_default_allocator();
    str.len = len;
    str.bytes = malloc(len);
    __CPROVER_assume(str.bytes != NULL);
    for (size_t i = 0; i < len; i++) {
        str.bytes[i] = nondet_uint8_t();
    }
    __CPROVER_assume(aws_string_is_valid(&str));

    /* Non-deterministic initialization of byte buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    bool result = aws_string_eq_byte_buf(&str, &buf);

    /* Postconditions */
    assert(aws_string_is_valid(&str));
    assert(aws_byte_buf_is_valid(&buf));
}
