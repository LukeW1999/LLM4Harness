#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_string_bytes_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a buffer large enough for the header and the bytes (including the null terminator) */
    char buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE + 1];
    struct aws_string *str = (struct aws_string *)buf;

    /* Initialize the string */
    str->allocator = aws_default_allocator();
    str->len = len;
    str->bytes[len] = '\0';

    /* Assume the string is valid (checks allocator, length, null terminator, etc.) */
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *const old_allocator = str->allocator;
    const size_t old_len = str->len;

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(aws_string_is_valid(str));
}
