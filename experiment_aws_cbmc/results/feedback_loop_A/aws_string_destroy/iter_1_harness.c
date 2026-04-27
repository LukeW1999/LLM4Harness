#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = malloc(sizeof(struct aws_string) + sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    str->allocator = (struct aws_allocator *)nondet_ptr();
    str->len = nondet_size_t();
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len + 1)); // Null terminator

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = *str;

    /* 3. Call function under test */
    aws_string_destroy(str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    // Since aws_string_destroy does not return a value, we assume it always succeeds in terms of operation
    // The main effect is the deallocation of memory, which we cannot directly assert in CBMC

    /* 5. Assert fields that must NOT change regardless of result */
    // In this case, the structure is freed, so we cannot assert fields of str directly
    // Instead, we assert that the allocator and len were as expected before the call
    assert(old.allocator == str->allocator);
    assert(old.len == str->len);

    /* 6. Assert validity invariants always holds */
    // After destruction, the structure is no longer valid, so we cannot assert aws_string_is_valid(&str)
    // Instead, we assert that the memory was properly released if an allocator was present
    if (old.allocator) {
        assert(AWS_MEM_IS_INVALID(old.bytes, old.len + 1)); // Null terminator
    }
}
