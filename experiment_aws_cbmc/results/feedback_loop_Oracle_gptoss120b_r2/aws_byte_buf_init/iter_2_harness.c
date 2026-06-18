#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/* Stub for aws_mem_acquire: nondeterministically succeed or fail */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    if (__CPROVER_nondet_bool()) {
        return __CPROVER_allocate(size);
    } else {
        return NULL;
    }
}

/* Stub for aws_default_allocator (returns a non‑NULL allocator) */
struct aws_allocator *aws_default_allocator(void) {
    static struct aws_allocator dummy_allocator;
    return &dummy_allocator;
}

/* Stub for aws_byte_buf_is_valid (the real predicate is simple) */
bool aws_byte_buf_is_valid(const struct aws_byte_buf *buf) {
    if (buf == NULL) {
        return false;
    }
    if (buf->len > buf->capacity) {
        return false;
    }
    if (buf->capacity != 0 && buf->buffer == NULL) {
        return false;
    }
    return true;
}

void aws_byte_buf_init_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity = __CPROVER_nondet_size_t();
    __CPROVER_assume(capacity <= 1024);

    unsigned char other_mem[256];
    unsigned char other_mem_before[256];
    for (size_t i = 0; i < 256; ++i) {
        other_mem[i] = __CPROVER_nondet_uchar();
        other_mem_before[i] = other_mem[i];
    }

    struct aws_byte_buf old_buf = buf;

    int ret = aws_byte_buf_init(&buf, allocator, capacity);

    if (capacity == 0) {
        assert(buf.buffer == NULL);
        assert(ret == AWS_OP_SUCCESS);
    } else {
        if (buf.buffer == NULL) {
            assert(ret == AWS_OP_ERR);
        } else {
            assert(ret == AWS_OP_SUCCESS);
        }
    }

    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == allocator);
    if (capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        if (ret == AWS_OP_SUCCESS) {
            assert(buf.buffer != NULL);
        }
    }
    assert(aws_byte_buf_is_valid(&buf));

    for (size_t i = 0; i < 256; ++i) {
        assert(other_mem[i] == other_mem_before[i]);
    }
}
