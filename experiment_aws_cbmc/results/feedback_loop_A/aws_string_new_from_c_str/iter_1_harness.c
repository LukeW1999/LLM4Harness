#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = (struct aws_allocator *)nondet_ptr();
    const char *c_str = (const char *)nondet_ptr();
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1)); // c_str must be null-terminated
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE); // bound the length of c_str

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string *old_result = NULL;

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result != NULL) {
        // Success path
        assert(result->allocator == allocator);
        assert(result->len == c_str_len);
        assert(AWS_MEM_IS_READABLE(result->bytes, c_str_len + 1)); // result->bytes must be null-terminated
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);
    } else {
        // Failure path
        assert(result == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as the function returns a new allocation

    /* 6. Assert validity invariants always holds */
    if (result != NULL) {
        assert(aws_string_is_valid(result));
    }
}
