#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    /*  1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_old_byte;
    if (src.buffer != NULL) {
        save_byte_from_array(src.buffer, src.capacity, &src_old_byte);
    }

    /* Allocator – may fail, nondeterministically */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Declare dest, uninitialized */
    struct aws_byte_buf dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* success: dest takes the lengths/capacity from src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert(dest.allocator == allocator);

        if (src.buffer != NULL) {
            /* src had a real buffer – dest must have an allocated copy */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* src.buffer was NULL → dest is zeroed (except allocator) */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* failure: dest is zeroed (including allocator) */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
        /* zeroed buf is valid */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 5. Assert that src did not change */
    assert(old_src.len == src.len);
    assert(old_src.capacity == src.capacity);
    assert(old_src.allocator == src.allocator);
    assert(old_src.buffer == src.buffer);
    if (src.buffer != NULL) {
        assert_byte_from_buffer_matches(src.buffer, &src_old_byte);
    }

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&src));
}
