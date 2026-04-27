#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *src = (struct aws_string *)malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
    __CPROVER_assume(src != NULL);
    src->allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(src->allocator != NULL);
    src->len = nondet_size_t();
    __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
    src->bytes = (const uint8_t *)(src + 1);
    __CPROVER_assume(AWS_MEM_IS_READABLE(src->bytes, src->len));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = *src;

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (src != NULL) {
        assert(result.ptr == src->bytes);
        assert(result.len == src->len);
    } else {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(src->allocator == old.allocator);
    assert(src->len == old.len);
    assert(src->bytes == old.bytes);

    /* 6. Assert validity invariants always holds */
    if (src != NULL) {
        assert(AWS_MEM_IS_READABLE(src->bytes, src->len));
    }
}
