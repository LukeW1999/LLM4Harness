#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* parameters */
    const char *c_str = ensure_c_str_is_allocated(MAX_STRING_LEN);
    struct aws_allocator *allocator;

    /* assumptions */
    __CPROVER_assume(c_str != NULL);
    ASSUME_DEFAULT_ALLOCATOR(allocator);

    /* operation under verification */
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    /* assertions */
    if (str) {
        assert(str->allocator == allocator);
        assert(str->len == strlen(c_str));
        assert(str->bytes[str->len] == 0);
        assert_bytes_match(str->bytes, c_str, str->len);
        assert(aws_string_is_valid(str));
        aws_string_destroy(str);
    } else {
        assert(aws_last_error() != AWS_ERROR_SUCCESS);
    }

    assert(aws_c_string_is_valid(c_str));
}
