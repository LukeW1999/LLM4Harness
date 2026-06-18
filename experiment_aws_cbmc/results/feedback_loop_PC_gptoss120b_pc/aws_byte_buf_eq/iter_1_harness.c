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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src,  MAX_BUFFER_SIZE));

    /* src must have a concrete buffer if its capacity > 0 */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* allocator used by the function */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect src */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* buffer must be allocated and contents must match */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on allocation failure dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must be unchanged */
    assert(src.buffer   == old_src.buffer);
    assert(src.len      == old_src.len);
    assert(src.capacity == old_src.capacity);
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* dest fields that are not specified to change on success must remain unchanged on failure */
    if (result == AWS_OP_ERR) {
        /* when the function fails, dest is zeroed, so we only need to check that it does not retain old values */
        assert(dest.buffer   == NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    } else {
        /* on success, fields not mentioned (e.g., any padding) are irrelevant; we have already asserted the relevant ones */
        (void)old_dest; /* silence unused variable warning */
    }

    /* 6. Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
