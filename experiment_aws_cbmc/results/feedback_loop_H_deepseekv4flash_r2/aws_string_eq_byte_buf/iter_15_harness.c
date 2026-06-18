#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 100
#define MAX_BUFFER_SIZE 100

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic allocation and initialization of string */
    size_t len;
    __CPROVER_assume(len > 0 && len < MAX_STRING_LEN);
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    str->allocator = aws_default_allocator();
    str->len = len;
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_byte();
    }
    str->bytes[len] = '\0';
    __CPROVER_assume(aws_string_is_valid(str));

    /* Non-deterministic initialization of byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postconditions */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(&buf));
    (void)result;
}
