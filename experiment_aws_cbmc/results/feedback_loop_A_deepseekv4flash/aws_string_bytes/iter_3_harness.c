#include <aws/common/string.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_STRING_LEN 10

void aws_string_bytes_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    // Allocate memory for the string structure, including the flexible array member
    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    // Initialize the allocator and length fields (cast away const for modeling)
    struct aws_allocator *alloc = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    if (alloc != NULL) {
        *(struct aws_allocator **)&str->allocator = alloc;
    } else {
        *(struct aws_allocator **)&str->allocator = NULL;
    }
    *(size_t *)&str->len = len;

    // Fill the bytes array with nondeterministic data
    for (size_t i = 0; i < len; i++) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    // Save a copy of the structure for immutability checks
    struct aws_string old = *str;

    // Call the function under test
    const uint8_t *result = aws_string_bytes(str);

    // Postconditions
    assert(result == str->bytes);
    assert(result != NULL);

    // Check that the string structure is unchanged
    assert(str->len == old.len);
    assert(str->allocator == old.allocator);

    // Check that the byte content is unchanged
    for (size_t i = 0; i < len; i++) {
        assert(str->bytes[i] == old.bytes[i]);
    }
}
