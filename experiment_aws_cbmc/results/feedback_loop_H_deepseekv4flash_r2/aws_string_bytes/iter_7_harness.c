#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    struct aws_string *str = make_aws_string();
    __CPROVER_assume(aws_string_is_valid(str));
    const uint8_t *result = aws_string_bytes(str);
    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
}
