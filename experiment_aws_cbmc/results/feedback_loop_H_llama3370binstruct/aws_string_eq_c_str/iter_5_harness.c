#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));
    const char *c_str = (const char *)nondet_uint8_t();

    bool result = aws_string_eq_c_str(&str, c_str);
    assert(result == (aws_string_eq(&str, aws_string_from_c_str(aws_default_allocator(), c_str))));

    struct aws_string empty_str;
    aws_string_init_from_array(&empty_str, "", 0);
    assert(aws_string_eq_c_str(&empty_str, "") == true);

    struct aws_string null_str;
    aws_string_init(&null_str, aws_default_allocator(), 0);
    assert(aws_string_eq_c_str(&null_str, NULL) == false);

    struct aws_string str2;
    aws_string_init(&str2, aws_default_allocator(), 10);
    str2.len = 10;
    str2.data[0] = 'a';
    str2.data[1] = 'b';
    str2.data[2] = 'c';
    str2.data[3] = 'd';
    str2.data[4] = 'e';
    str2.data[5] = 'f';
    str2.data[6] = 'g';
    str2.data[7] = 'h';
    str2.data[8] = 'i';
    str2.data[9] = '\0';
    assert(aws_string_eq_c_str(&str2, "abcdefghi") == true);

    struct aws_string str3;
    aws_string_init(&str3, aws_default_allocator(), 10);
    str3.len = 10;
    str3.data[0] = 'a';
    str3.data[1] = 'b';
    str3.data[2] = 'c';
    str3.data[3] = 'd';
    str3.data[4] = 'e';
    str3.data[5] = 'f';
    str3.data[6] = 'g';
    str3.data[7] = 'h';
    str3.data[8] = 'i';
    str3.data[9] = 'j';
    assert(aws_string_eq_c_str(&str3, "abcdefghi") == false);
}
