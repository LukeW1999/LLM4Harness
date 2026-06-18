#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_string_bytes_harness() {
    /* ensure_aws_string_is_allocated allocates a string with length <= max bound */
    struct aws_string *str = ensure_aws_string_is_allocated(MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    /* The function must return the internal bytes buffer */
    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
}
