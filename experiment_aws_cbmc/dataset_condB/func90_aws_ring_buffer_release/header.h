#ifndef AWS_COMMON_RING_BUFFER_H
#define AWS_COMMON_RING_BUFFER_H

#include <aws/common/atomics.h>

AWS_PUSH_SANE_WARNING_LEVEL

struct aws_ring_buffer {
    struct aws_allocator *allocator;
    uint8_t *allocation;
    struct aws_atomic_var head;
    struct aws_atomic_var tail;
    uint8_t *allocation_end;
};

struct aws_byte_buf;

AWS_EXTERN_C_BEGIN

AWS_COMMON_API int aws_ring_buffer_init(struct aws_ring_buffer *ring_buf, struct aws_allocator *allocator, size_t size);

AWS_STATIC_IMPL bool aws_ring_buffer_check_atomic_ptr(
    const struct aws_ring_buffer *ring_buf,
    const uint8_t *atomic_ptr);

AWS_STATIC_IMPL bool aws_ring_buffer_is_empty(const struct aws_ring_buffer *ring_buf);

AWS_STATIC_IMPL bool aws_ring_buffer_is_valid(const struct aws_ring_buffer *ring_buf);

AWS_COMMON_API void aws_ring_buffer_clean_up(struct aws_ring_buffer *ring_buf);

AWS_COMMON_API int aws_ring_buffer_acquire(
    struct aws_ring_buffer *ring_buf,
    size_t requested_size,
    struct aws_byte_buf *dest);

AWS_COMMON_API int aws_ring_buffer_acquire_up_to(
    struct aws_ring_buffer *ring_buf,
    size_t minimum_size,
    size_t requested_size,
    struct aws_byte_buf *dest);

AWS_COMMON_API void aws_ring_buffer_release(struct aws_ring_buffer *ring_buffer, struct aws_byte_buf *buf);

AWS_COMMON_API bool aws_ring_buffer_buf_belongs_to_pool(
    const struct aws_ring_buffer *ring_buffer,
    const struct aws_byte_buf *buf);

AWS_EXTERN_C_END

#ifndef AWS_NO_STATIC_IMPL
#    include <aws/common/ring_buffer.inl>
#endif 

AWS_POP_SANE_WARNING_LEVEL

#endif
