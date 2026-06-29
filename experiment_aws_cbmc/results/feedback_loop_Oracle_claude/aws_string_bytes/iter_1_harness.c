#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* Allocate a non-deterministic aws_string */
    struct aws_string *str = ensure_string_is_allocated_nondet_length();

    /* Precondition: str must be a valid aws_string */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save state before function call for frame condition checks */
    size_t old_len = str->len;
    struct aws_allocator *old_allocator = str->allocator;

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Postcondition 1: Return value correctness
     * The returned pointer must equal str->bytes */
    assert(result == str->bytes);

    /* Postcondition 2: Return value is not NULL
     * Since aws_string_is_valid ensures str is valid and bytes is part of the struct,
     * the result should be non-NULL */
    assert(result != NULL);

    /* Postcondition 3: The returned pointer points to valid memory
     * We can dereference the first byte (bytes[0] always exists due to the flexible array member) */
    /* The bytes array is accessible - we can check the null terminator at position len */
    assert(result[str->len] == '\0');

    /* Postcondition 4: Frame condition - str->len was not modified */
    assert(str->len == old_len);

    /* Postcondition 5: Frame condition - str->allocator was not modified */
    assert(str->allocator == old_allocator);

    /* Postcondition 6: The string is still valid after the call */
    assert(aws_string_is_valid(str));
}

void aws_string_bytes_harness(void) {
    aws_string_bytes_harness();
    return 0;
}
