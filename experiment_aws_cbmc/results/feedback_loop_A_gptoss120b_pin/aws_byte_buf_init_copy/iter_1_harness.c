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

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* nondeterministically make src a zero‑length buffer without allocation */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Declare destination buffer (output) */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest does not need to be pre‑initialized; function overwrites it */

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, aws_default_allocator(), &src);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* src empty → dest zeroed but allocator set */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == aws_default_allocator());
        } else {
            /* allocation succeeded → dest mirrors src (except allocator) */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == aws_default_allocator());
            assert(dest.buffer != NULL);
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        }
    } else {
        /* Allocation failure → dest zeroed, allocator NULL */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 5. src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
