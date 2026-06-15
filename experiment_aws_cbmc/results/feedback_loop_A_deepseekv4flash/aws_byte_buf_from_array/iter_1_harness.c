#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

static void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    if (nondet_bool()) {
        return malloc(size);
    }
    return NULL;
}

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator;

    /* nondet pointer for allocator, assume non‑null as required by preconditions */
    allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    /* restrict src buffer size to avoid huge loops */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* nondeterministically set src fields, then ensure validity */
    src.len = nondet_size_t();
    src.capacity = nondet_size_t();

    /* ensure consistency with the validity predicate */
    if (src.capacity == 0) {
        __CPROVER_assume(src.len == 0);
        src.buffer = NULL;
    } else {
        src.buffer = malloc(src.capacity);
        __CPROVER_assume(src.buffer != NULL);
        __CPROVER_assume(src.len <= src.capacity);
    }
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save src state for immutability check */
    struct aws_byte_buf old_src = src;

    /* call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postcondition 1: dest is always valid */
    assert(aws_byte_buf_is_valid(&dest));

    /* postcondition 2: src must remain valid (implied by const) */
    assert(aws_byte_buf_is_valid(&old_src));

    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* success with null source: dest zeroed except allocator */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == allocator);
        } else {
            /* success with non‑null source */
            assert(dest.allocator == allocator);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer); /* new allocation */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* failure: dest is zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* immutability of src fields (none of them should change) */
    assert(src.allocator == old_src.allocator);
    assert(src.buffer    == old_src.buffer);
    assert(src.len       == old_src.len);
    assert(src.capacity  == old_src.capacity);
}
