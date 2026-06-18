#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    struct aws_string *str = ensure_string_is_allocated(len);
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    assert(result == (const uint8_t *)str->bytes);
    assert(aws_string_is_valid(str));
}
