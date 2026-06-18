#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* set up source byte buffer */
    struct aws_byte_buf src;
    bool has_buffer = nondet_bool();
    if (has_buffer) {
        ensure_byte_buf_has_allocated_buffer_member(&src);
        __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
        __CPROVER_assume(aws_byte_buf_is_valid(&src));
    } else {
        src.allocator = NULL;
        src.buffer = NULL;
        src.len = 0;
        src.capacity = 0;
    }

    /* save old state of src for immutability check */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    if (old_src.buffer != NULL && old_src.len > 0) {
        save_byte_from_array(old_src.buffer, old_src.len, &src_storage);
    }

    struct aws_byte_buf dest;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* assert src unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    if (old_src.buffer != NULL && old_src.len > 0) {
        assert_byte_from_buffer_matches(old_src.buffer, &src_storage);
    }

    if (result == AWS_OP_SUCCESS) {
        /* success: dest is valid and properly initialized */
        assert(aws_byte_buf_is_valid(&dest));

        if (src.buffer != NULL) {
            /* non-null src buffer: dest has new backing array, contents copy */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* src buffer was NULL: dest zeroed but allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        }
    } else {
        /* failure: only possible when allocation failed (src.buffer must have been non-NULL) */
        assert(result == AWS_OP_ERR);
        assert(src.buffer != NULL);
        /* dest is zeroed entirely */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
