#include <aws/common/string.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_string_bytes_harness() {
    struct aws_string *str;
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
}
