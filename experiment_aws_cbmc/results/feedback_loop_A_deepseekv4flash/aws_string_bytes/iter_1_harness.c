#include <aws/common/string.h>

void aws_string_bytes_harness() {
    // Bounds for length to keep state space manageable
    size_t len;
    __CPROVER_assume(len < MAX_BUFFER_SIZE);

    // Allocate memory for the string struct plus the bytes array
    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    // Set fields: cast away const to initialize
    // allocator can be NULL (static string) or a nondet pointer
    struct aws_allocator *alloc = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    *(struct aws_allocator **)&str->allocator = alloc;
    *(size_t *)&str->len = len;

    // Fill bytes with nondet data
    for (size_t i = 0; i < len; i++) {
        ((uint8_t *)&str->bytes)[i] = nondet_uint8_t();
    }

    // Ensure the string is valid according to its validity predicate
    __CPROVER_assume(aws_string_is_valid(str));

    // Save old state for immutability checks
    struct aws_string old = *str;

    // Call function under test
    const uint8_t *result = aws_string_bytes(str);

    // Postcondition: returned pointer equals str->bytes
    assert(result == str->bytes);
    assert(result != NULL); // bytes are at a non-null address

    // Check unchanged fields
    assert(str->len == old.len);
    assert(str->allocator == old.allocator);
    assert(str->bytes[0] == old.bytes[0]);

    // Validity invariant still holds
    assert(aws_string_is_valid(str));
}
