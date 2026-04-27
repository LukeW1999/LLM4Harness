#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    /* parameters */
    const char *c_str;

    /* assumption */
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, strlen(c_str) + 1));

    /* operation under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* assertions */
    assert(aws_byte_cursor_is_valid(&cur));
    assert(cur.len == (c_str ? strlen(c_str) : 0));
    assert(cur.ptr == (uint8_t *)c_str);
}
