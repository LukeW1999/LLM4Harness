#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = bounded_malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
    __CPROVER_assume(str != NULL);
    str->allocator = can_fail_allocator();
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
    save_byte_from_array(str->bytes, str->len, &str->bytes_storage);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = *str;

    /* 3. Call function under test */
    aws_string_destroy_secure(str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (str) {
        assert_byte_from_buffer_matches(str->bytes, &str->bytes_storage);
        assert(str->allocator == old.allocator);
        assert(str->len == old.len);
    } else {
        assert(str == old);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&old));
}
