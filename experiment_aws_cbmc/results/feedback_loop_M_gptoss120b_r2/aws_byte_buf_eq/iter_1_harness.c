#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* allocator – use the default allocator as required by the library */
    struct aws_allocator *alloc = aws_default_allocator();

    /* source buffer – nondeterministic but bounded */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer – uninitialized (output) */
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest;   /* capture pre‑state for failure case */

    /* preserve source state – it must remain unchanged */
    struct aws_byte_buf old_src = src;

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* post‑condition: the destination must always be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        /* on success the allocator is set to the provided allocator */
        assert(dest.allocator == alloc);
        /* length and capacity are copied from the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        /* buffer handling depends on whether the source had a buffer */
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            /* the contents up to src.len must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* destination must be unchanged from its pre‑state (which is zeroed by the function) */
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* source buffer must remain unchanged regardless of the outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
}
