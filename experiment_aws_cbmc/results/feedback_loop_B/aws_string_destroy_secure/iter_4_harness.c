#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    str.allocator = aws_default_allocator();
    str.len = nondet_size_t();
    __CPROVER_assume(str.len <= MAX_BUFFER_SIZE);
    str.bytes = bounded_malloc(str.len);
    __CPROVER_assume(str.bytes != NULL);
    save_byte_from_array(str.bytes, str.len, &str.bytes_storage);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    aws_string_destroy_secure(&str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert_byte_from_buffer_matches(old.bytes, &old.bytes_storage);
    assert(old.allocator == aws_default_allocator());

    /* 5. Assert fields that must NOT change regardless of result */
    assert(old.allocator == aws_default_allocator());
    assert(old.len == str.len);

    /* 6. Assert validity invariant always holds */
    assert(!aws_string_is_valid(&str));

    /* 7. Assert that the memory has been freed */
    assert(str.bytes == NULL);
}
