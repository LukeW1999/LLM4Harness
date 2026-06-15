#include <aws/common/string.h>

#define MAX_BUFFER_SIZE 10

void aws_string_bytes_harness() {
    size_t len;
    __CPROVER_assume(len < MAX_BUFFER_SIZE);

    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    // Initialize fields (cast away const for modeling)
    struct aws_allocator *alloc = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    *(struct aws_allocator **)&str->allocator = alloc;
    *(size_t *)&str->len = len;

    // Fill bytes with nondet data (loop bound manageable due to MAX_BUFFER_SIZE)
    for (size_t i = 0; i < len; i++) {
        ((uint8_t *)&str->bytes)[i] = nondet_uint8_t();
    }

    // Save a copy for immutability checks
    struct aws_string old = *str;

    // Call the function under test
    const uint8_t *result = aws_string_bytes(str);

    // Postconditions
    assert(result == str->bytes);
    assert(result != NULL);

    // Check that the string structure is unchanged
    assert(str->len == old.len);
    assert(str->allocator == old.allocator);

    // Check that byte content is unchanged (only if length > 0 to avoid out‑of‑bounds)
    if (len > 0) {
        assert(str->bytes[0] == old.bytes[0]);
    }
}
