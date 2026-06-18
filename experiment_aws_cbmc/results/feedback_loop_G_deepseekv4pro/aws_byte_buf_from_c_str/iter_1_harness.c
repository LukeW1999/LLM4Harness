#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness() {
    // Allocate a bounded, null‑terminated character array for non‑NULL input.
    char c_str_buf[MAX_BUFFER_SIZE + 1];
    // Fill with non‑deterministic bytes.
    for (size_t i = 0; i <= MAX_BUFFER_SIZE; i++) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    // Enforce null termination at the end of the buffer.
    c_str_buf[MAX_BUFFER_SIZE] = '\0';

    // Decide whether c_str is NULL or points to the buffer.
    char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        c_str = c_str_buf;
    }

    // Save a representative byte of the input string (if non‑NULL) to verify immutability.
    struct store_byte_from_buffer old_byte;
    if (c_str != NULL) {
        // The function must not modify any byte up to and including the null terminator.
        size_t safe_len = strlen(c_str);
        save_byte_from_array((const uint8_t *)c_str, safe_len + 1, &old_byte);
    }

    // Call the function under test.
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    // ---- Postconditions ----
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        if (expected_len == 0) {
            assert(result.buffer == NULL);
        } else {
            assert(result.buffer == (uint8_t *)c_str);
        }
        // Verify the original string was not modified.
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_byte);
    }

    // Allocator is always NULL.
    assert(result.allocator == NULL);

    // The returned byte_buf must satisfy the structural validity predicate.
    assert(aws_byte_buf_is_valid(&result));
}
