#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness(void) {
    /* Allocate and nondeterministically initialize src */
    struct aws_byte_buf src;
    src.allocator = NULL;

    size_t src_capacity = nondet_size_t();
    __CPROVER_assume(src_capacity <= SIZE_MAX / 2);

    bool src_has_buffer = nondet_bool();
    if (src_has_buffer) {
        src.buffer = malloc(src_capacity);
        __CPROVER_assume(src.buffer != NULL);
        src.capacity = src_capacity;
        src.len = nondet_size_t();
        __CPROVER_assume(src.len <= src.capacity);
        for (size_t i = 0; i < src.capacity; ++i) {
            src.buffer[i] = nondet_uint8_t();
        }
    } else {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Backup src for frame condition */
    struct aws_byte_buf src_old = src;
    uint8_t *src_buffer_old = NULL;
    if (src.buffer) {
        src_buffer_old = malloc(src.capacity);
        __CPROVER_assume(src_buffer_old != NULL);
        for (size_t i = 0; i < src.capacity; ++i) {
            src_buffer_old[i] = src.buffer[i];
        }
    }

    /* Allocate dest (uninitialized) */
    struct aws_byte_buf dest;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call the function under verification */
    int ret = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (src.buffer == NULL) {
        assert(ret == AWS_OP_SUCCESS);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
    } else {
        if (ret == AWS_OP_SUCCESS) {
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == allocator);
            for (size_t i = 0; i < src.len; ++i) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        } else {
            assert(ret == AWS_OP_ERR);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == NULL);
        }
    }

    /* Frame condition: src unchanged */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);
    if (src.buffer) {
        for (size_t i = 0; i < src.capacity; ++i) {
            assert(src.buffer[i] == src_buffer_old[i]);
        }
    }

    /* Clean up allocated memory */
    if (src.buffer) {
        free(src.buffer);
    }
    if (src_buffer_old) {
        free(src_buffer_old);
    }
    if (dest.buffer) {
        aws_mem_release(allocator, dest.buffer);
    }
}
