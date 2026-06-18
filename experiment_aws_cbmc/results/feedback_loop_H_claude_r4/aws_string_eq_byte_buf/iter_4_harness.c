#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void harness(void) {
    /* Set up aws_string */
    struct aws_string *str = make_arbitrary_aws_string_nondet_len();
    __CPROVER_assume(aws_string_is_valid(str));

    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* Postconditions */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(&buf));
}
