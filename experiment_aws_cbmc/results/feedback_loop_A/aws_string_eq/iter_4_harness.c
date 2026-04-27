#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t bytes_a[MAX_BUFFER_SIZE];
    uint8_t bytes_b[MAX_BUFFER_SIZE];

    struct aws_string a = {
        .allocator = allocator,
        .len = len_a,
        .bytes = bytes_a
    };

    struct aws_string b = {
        .allocator = allocator,
        .len = len_b,
        .bytes = bytes_b
    };

    ensure_memory_is_initialized(bytes_a, len_a);
    ensure_memory_is_initialized(bytes_b, len_b);

    /* Add preconditions for aws_string_is_valid */
    __CPROVER_assume(aws_string_is_valid(&a));
    __CPROVER_assume(aws_string_is_valid(&b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_a = a;
    struct aws_string old_b = b;

    /* 3. Call function under test */
    bool result = aws_string_eq(&a, &b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(a.len == b.len);
        __CPROVER_assume(a.len == b.len); // Ensure lengths are equal for comparison
        assert_bytes_match(a.bytes, b.bytes, a.len);
    } else {
        /* No specific changes guaranteed on failure, but inputs should be unchanged */
        assert(a.len == old_a.len);
        assert(b.len == old_b.len);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a.allocator == old_a.allocator);
    assert(b.allocator == old_b.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&a));
    assert(aws_string_is_valid(&b));
}
