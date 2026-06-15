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
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* nondeterministically decide whether src.buffer is NULL */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.len = 0;
        src.capacity = 0;
    } else {
        /* ensure len does not exceed capacity */
        if (src.capacity > 0) {
            src.len = nondet_size_t();
            __CPROVER_assume(src.len <= src.capacity);
        } else {
            src.len = 0;
        }
    }
    src.allocator = allocator; /* allocator field not relevant for validity */

    /* save old src for immutability checks */
    struct aws_byte_buf old_src = src;
    if (src.buffer != NULL && src.len > 0) {
        /* store a copy of the source bytes for later comparison */
        struct store_byte_from_buffer src_bytes;
        save_byte_from_array(src.buffer, src.len, &src_bytes);
    }

    /* destination buffer */
    struct aws_byte_buf dest;
    struct aws_byte_buf old_dest = dest; /* capture uninitialized old state */

    /* call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        if (src.buffer == NULL) {
            /* when src is empty, dest is zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* dest must have a new allocation */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.allocator == allocator);
            /* contents must match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on failure dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* zeroed dest should still satisfy validity predicate */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(old_src.buffer, old_src.len, &old_bytes);
        assert_byte_from_buffer_matches(src.buffer, &old_bytes);
    }

    /* overall invariants */
    assert(aws_byte_buf_is_valid(&src));
}
