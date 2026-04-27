#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *src = bounded_malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
    __CPROVER_assume(src != NULL);
    src->allocator = can_fail_allocator();
    src->len = nondet_size_t();
    __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
    ensure_memory_is_initialized((void *)(src->bytes), src->len);
    src->bytes[src->len] = '\0';  // Ensure null-termination

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = *src;

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (src != NULL) {
        assert(cursor.ptr == src->bytes);
        assert(cursor.len == src->len);
    } else {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(src->allocator == old.allocator);
    assert(src->len == old.len);
    assert(AWS_MEM_IS_READABLE(src->bytes, src->len));

    /* 6. Assert validity invariants always holds */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
    assert(aws_byte_cursor_is_valid(&cursor));
}
