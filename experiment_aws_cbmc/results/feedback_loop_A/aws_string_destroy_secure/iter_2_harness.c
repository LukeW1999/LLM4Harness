#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_allocator *allocator = aws_default_allocator();
    str.allocator = allocator;
    str.len = nondet_size_t();
    __CPROVER_assume(str.len <= MAX_BUFFER_SIZE);
    str.bytes = can_fail_malloc(sizeof(uint8_t) * str.len);
    __CPROVER_assume(str.bytes != NULL);
    for (size_t i = 0; i < str.len; i++) {
        str.bytes[i] = nondet_uint8_t();
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    aws_string_destroy_secure(&str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path: str is deallocated and bytes are zeroed */
    if (old.allocator != NULL) {
        assert(old.allocator == allocator); /* Allocator should remain the same */
        assert(AWS_MEM_IS_ZEROED(old.bytes, old.len)); /* Bytes should be zeroed */
    } else {
        /* Failure path: str remains unchanged */
        assert(str == old);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to assert as str is deallocated or remains unchanged */

    /* 6. Assert validity invariants always holds */
    /* Since str is deallocated, we cannot assert aws_string_is_valid(&str) */
}
