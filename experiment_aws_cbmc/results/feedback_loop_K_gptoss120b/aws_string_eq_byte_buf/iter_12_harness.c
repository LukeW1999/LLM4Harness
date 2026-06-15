#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t max_len = 64;
    size_t len = __CPROVER_nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* Create a nondet byte array for the string data */
    uint8_t *str_data = NULL;
    if (len > 0) {
        str_data = malloc(len);
        __CPROVER_assume(str_data != NULL);
        __CPROVER_assume(__CPROVER_is_fresh(str_data, len));
    }

    struct aws_string *str = aws_string_new_from_array(allocator, str_data, len);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Initialize a byte buffer with sufficient capacity */
    struct aws_byte_buf buf;
    aws_byte_buf_init(&buf, allocator, max_len);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    buf.len = len;
    __CPROVER_assume(buf.buffer != NULL || len == 0);

    bool result = aws_string_eq_byte_buf(str, &buf);

    bool expected = false;
    if (str->len == buf.len) {
        expected = (memcmp(str->bytes, buf.buffer, str->len) == 0);
    }
    assert(result == expected);
}
