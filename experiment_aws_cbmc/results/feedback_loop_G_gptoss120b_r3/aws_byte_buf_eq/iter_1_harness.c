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
    /* allocator – always the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* src – nondeterministic but bounded and valid */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old src for immutability checks */
    struct aws_byte_buf old_src = src;

    /* dest – output buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* allocator must be the default allocator */
    assert(allocator == aws_default_allocator());

    if (result == AWS_OP_SUCCESS) {
        /* allocator field is set to the supplied allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* Empty source case – dest should be an empty buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Non‑empty source – dest must mirror src's size and contents */
            assert(dest.buffer != NULL);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }

        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* Zeroed buffer is still considered valid */
        assert(aws_byte_buf_is_valid(&dest));
    }
}
