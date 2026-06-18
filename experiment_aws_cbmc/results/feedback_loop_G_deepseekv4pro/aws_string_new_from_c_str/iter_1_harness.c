#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    // Use the default allocator (non‑NULL, valid)
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    // Non‑deterministic length for the input C string, bounded to keep state space small
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    // Allocate a buffer for the C string (including null terminator)
    char *buf = (char *)malloc(len + 1);
    __CPROVER_assume(buf != NULL);

    // Fill the buffer with arbitrary bytes
    for (size_t i = 0; i < len; i++) {
        buf[i] = (char)nondet_uint8_t();
    }
    buf[len] = '\0';   // guarantee null termination

    const char *c_str = buf;

    // Save the original contents of the C string buffer for later immutability checks
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)c_str, len + 1, &old_byte);

    // Call the function under test
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    // Postconditions
    if (str != NULL) {
        // Success: the returned string must be valid
        assert(aws_string_is_valid(str));

        // The allocator stored inside the string must be the one we passed
        assert(str->allocator == allocator);

        // The length field must equal the length of the C string (excluding null terminator)
        assert(str->len == strlen(c_str));

        // The bytes of the new string must match the original C string up to str->len
        assert_bytes_match(aws_string_bytes(str), (const uint8_t *)c_str, str->len);
    } else {
        // Failure: the input C string must not have been modified
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_byte);
    }

    // Regardless of success or failure, the input C string must remain unchanged
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_byte);

    // Clean up to avoid memory leak warnings
    if (str != NULL) {
        aws_string_destroy(str);
    }
    free(buf);
}
