#include <assert.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf buf;
    buf.allocator = alloc;

    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    buf.capacity = capacity;

    if (capacity > 0) {
        buf.buffer = (uint8_t *)alloc->mem_acquire(alloc, capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    size_t len;
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    struct store_byte_from_buffer storage = {0};
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &storage);
    }

    aws_byte_buf_clean_up_secure(&buf);

    assert(buf.allocator == old.allocator);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    if (old.buffer != NULL && old.capacity > 0) {
        assert(storage.ptr != NULL);
        assert(*storage.ptr == 0);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
