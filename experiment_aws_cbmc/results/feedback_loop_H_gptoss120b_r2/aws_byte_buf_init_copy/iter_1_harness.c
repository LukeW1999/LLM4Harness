#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* allocator – use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Remember whether src had a buffer before the call */
    bool src_had_buffer = (src.buffer != NULL);

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (!src_had_buffer) {
            /* src.buffer == NULL case – dest is zeroed but allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* Normal copy case */
            assert(dest.allocator == allocator);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.buffer != NULL);
            /* Verify that the copied bytes match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failure – dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* Validity invariants must hold for both structures */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
