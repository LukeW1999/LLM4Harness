#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare and set up a valid aws_string */
    /* We need to allocate an aws_string with some bytes following it.
     * Use a bounded size for the string length. */
    size_t len;
    __CPROVER_assume(len <= 10); /* bound the string length */

    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialize the string fields - cast away const for initialization */
    *(size_t *)&str->len = len;
    /* allocator can be NULL or non-NULL */
    /* bytes are already allocated as part of the struct */

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 3. Assert postconditions */
    /* The function returns str->bytes, which is the address of the bytes field */
    assert(result == str->bytes);

    /* The result should be non-NULL since str is non-NULL and bytes is embedded in the struct */
    assert(result != NULL);

    /* The string fields should be unchanged */
    assert(str->len == len);

    /* The string should still be valid */
    assert(aws_string_is_valid(str));

    /* The result pointer should point to readable memory of at least len bytes */
    /* (plus null terminator) */
    if (len > 0) {
        __CPROVER_assert(AWS_MEM_IS_READABLE(result, len), "result bytes are readable");
    }
}
