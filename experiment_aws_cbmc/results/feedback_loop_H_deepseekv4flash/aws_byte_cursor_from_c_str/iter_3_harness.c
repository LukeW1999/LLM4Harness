#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

#define MAX_STRING_LEN 32

void aws_byte_cursor_from_c_str_harness() {
    char str[MAX_STRING_LEN];
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);

    // Fill the string with non-null characters up to len-1
    for (size_t i = 0; i < len; i++) {
        str[i] = nondet_uint8_t();
        __CPROVER_assume(str[i] != '\0');
    }
    str[len] = '\0';  // null terminator

    const char *c_str = str;  // always non-null

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    assert(cur.ptr == (uint8_t *)c_str);
    assert(cur.len == len);
    assert(aws_byte_cursor_is_valid(&cur));
}
