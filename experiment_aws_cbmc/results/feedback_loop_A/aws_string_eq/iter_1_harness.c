#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>
#include <cbmc_proof/proof_check_assert.h>

void aws_string_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *a = bounded_malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
    struct aws_string *b = bounded_malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);

    if (a) {
        a->allocator = can_fail_allocator();
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= MAX_BUFFER_SIZE);
        a->bytes = (uint8_t *)(a + 1);
        ensure_memory_is_initialized(a->bytes, a->len);
    }

    if (b) {
        b->allocator = can_fail_allocator();
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= MAX_BUFFER_SIZE);
        b->bytes = (uint8_t *)(b + 1);
        ensure_memory_is_initialized(b->bytes, b->len);
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_a = *a;
    struct aws_string old_b = *b;

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(a->len == b->len);
        assert_bytes_match(a->bytes, b->bytes, a->len);
    } else {
        /* No specific changes guaranteed on failure, but inputs should be unchanged */
        assert(a->len == old_a.len);
        assert(a->bytes == old_a.bytes);
        assert(b->len == old_b.len);
        assert(b->bytes == old_b.bytes);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a->allocator == old_a.allocator);
    assert(b->allocator == old_b.allocator);

    /* 6. Assert validity invariant always holds */
    if (a) {
        assert(aws_string_is_valid(a));
    }
    if (b) {
        assert(aws_string_is_valid(b));
    }
}
