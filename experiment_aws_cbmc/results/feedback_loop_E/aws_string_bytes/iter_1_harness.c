#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_bytes_harness() {
    /* parameters */
    struct aws_string *str;

    /* assumptions */
    str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN * sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, MAX_STRING_LEN));
    __CPROVER_assume(str->len <= MAX_STRING_LEN);
    __CPROVER_assume(aws_string_is_valid(str));

    /* save old state */
    struct aws_string old = *str;

    /* operation under verification */
    const uint8_t *result = aws_string_bytes(str);

    /* assertions */
    assert(result == str->bytes);
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert(AWS_MEM_IS_READABLE(result, str->len));
    assert(aws_string_is_valid(str));
}
