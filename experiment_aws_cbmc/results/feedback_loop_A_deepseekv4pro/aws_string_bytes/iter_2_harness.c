#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_string_bytes_harness() {
    struct aws_string *str;

    /* Ensure the aws_string is fully allocated and valid, including a readable bytes buffer */
    ensure_aws_string_is_allocated_nondet_length(&str, 1);

    /* Save original fields for immutability checks */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes = str->bytes;

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Postcondition: result is exactly the bytes member of str */
    assert(result == str->bytes);

    /* The string struct is immutable, all fields must remain unchanged */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes);

    /* The returned pointer points to readable memory of the expected length */
    assert(result && AWS_MEM_IS_READABLE(result, str->len + 1));

    /* The whole string remains valid after the call */
    assert(aws_string_is_valid(str));
}
