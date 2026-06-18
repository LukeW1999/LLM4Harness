#include <stddef.h>
#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

/* Declaration of CBMC nondeterministic size_t generator */
size_t nondet_size_t(void);

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    size_t src_capacity = nondet_size_t();
    __CPROVER_assume(src_capacity <= MAX_BUFFER_SIZE);
    size_t src_len = nondet_size_t();
    __CPROVER_assume(src_len <= src_capacity);

    src.capacity = src_capacity;
    src.len = src_len;
    src.allocator = aws_default_allocator();

    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.capacity > 0) {
        save_byte_from_array(src.buffer, src.capacity, &src_byte);
    }

    struct aws_byte_buf old_src = src;

    struct aws_byte_buf dest = {0};
    struct aws_allocator *allocator = aws_default_allocator();

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            for (size_t i = 0; i < src.len; ++i) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        }
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL && src.capacity > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    assert(aws_byte_buf_is_valid(&src));
}
