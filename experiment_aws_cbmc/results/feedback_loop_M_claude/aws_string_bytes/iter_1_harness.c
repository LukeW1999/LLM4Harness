#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_bytes_harness() {
    /* 1. Allocate and set up a valid aws_string */
    /* We need to allocate memory for the struct + bytes inline */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    /* The struct has bytes[1] at the end, so total size = sizeof(struct aws_string) + len */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialize the fields - need to cast away const for initialization */
    *(size_t *)&str->len = len;
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 3. Assert postconditions */
    /* The function returns str->bytes, which is the address of the bytes field */
    assert(result == str->bytes);

    /* The result pointer should be non-null since str is non-null */
    assert(result != NULL);

    /* The string fields should be unchanged */
    assert(str->len == len);
    assert(str->allocator == aws_default_allocator());

    /* The string should still be valid */
    assert(aws_string_is_valid(str));

    /* The result should point to readable memory of at least len bytes */
    /* (plus null terminator) */
    if (len > 0) {
        __CPROVER_assert(AWS_MEM_IS_READABLE(result, len), "result bytes are readable");
    }
}
