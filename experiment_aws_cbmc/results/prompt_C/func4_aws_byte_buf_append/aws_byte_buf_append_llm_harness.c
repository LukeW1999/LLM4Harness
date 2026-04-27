#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <cbmc/model_assert.h>
#include <cbmc/proof_allocators.h>

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1
#define AWS_ERROR_DEST_COPY_TOO_SMALL 1

struct aws_allocator {
    void *mem_pool;
};

struct aws_byte_buf {
    size_t len;
    uint8_t *buffer;
    size_t capacity;
    struct aws_allocator *allocator;
};

struct aws_byte_cursor {
    size_t len;
    const uint8_t *ptr;
};

int aws_byte_buf_is_valid(const struct aws_byte_buf *buf) {
    return buf != NULL && buf->len <= buf->capacity && (buf->capacity == 0 || buf->buffer != NULL);
}

int aws_byte_cursor_is_valid(const struct aws_byte_cursor *cursor) {
    return cursor != NULL && (cursor->len == 0 || cursor->ptr != NULL);
}

int aws_raise_error(int error_code) {
    return AWS_OP_ERR;
}

int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from) {
    __CPROVER_precondition(aws_byte_buf_is_valid(to), "Precondition: aws_byte_buf_is_valid(to)");
    __CPROVER_precondition(aws_byte_cursor_is_valid(from), "Precondition: aws_byte_cursor_is_valid(from)");

    if (to->capacity - to->len < from->len) {
        __CPROVER_assert(aws_byte_buf_is_valid(to), "Postcondition: aws_byte_buf_is_valid(to)");
        __CPROVER_assert(aws_byte_cursor_is_valid(from), "Postcondition: aws_byte_cursor_is_valid(from)");
        return aws_raise_error(AWS_ERROR_DEST_COPY_TOO_SMALL);
    }

    if (from->len > 0) {
        __CPROVER_assert(from->ptr, "Assertion: from->ptr is not null");
        __CPROVER_assert(to->buffer, "Assertion: to->buffer is not null");
        memcpy(to->buffer + to->len, from->ptr, from->len);
        to->len += from->len;
    }

    __CPROVER_assert(aws_byte_buf_is_valid(to), "Postcondition: aws_byte_buf_is_valid(to)");
    __CPROVER_assert(aws_byte_cursor_is_valid(from), "Postcondition: aws_byte_cursor_is_valid(from)");
    return AWS_OP_SUCCESS;
}

void aws_byte_buf_append_harness() {
    /* 1. Declare data structures */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;
    uint8_t to_buffer[100];
    uint8_t from_buffer[100];

    /* 2. Bound and allocate structures */
    to.buffer = to_buffer;
    from.ptr = from_buffer;

    __CPROVER_assume(to.capacity <= sizeof(to_buffer));
    __CPROVER_assume(from.len <= sizeof(from_buffer));

    /* 3. Assume preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 4. Save old state if needed */
    size_t old_to_len = to.len;
    size_t old_from_len = from.len;
    uint8_t old_to_buffer[sizeof(to_buffer)];
    memcpy(old_to_buffer, to.buffer, to.capacity);

    /* 5. Call function under verification */
    int result = aws_byte_buf_append(&to, &from);

    /* 6. Assert postconditions (both success and failure branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to_len + old_from_len);
        assert(to.capacity == to.capacity);
        assert(to.allocator == to.allocator);
        assert(from.len == old_from_len);
        assert(from.ptr == from_buffer);
        assert(memcmp(to.buffer, old_to_buffer, old_to_len) == 0);
        assert(memcmp(to.buffer + old_to_len, from_buffer, old_from_len) == 0);
    } else if (result == AWS_OP_ERR) {
        assert(to.len == old_to_len);
        assert(to.capacity == to.capacity);
        assert(to.allocator == to.allocator);
        assert(from.len == old_from_len);
        assert(from.ptr == from_buffer);
        assert(memcmp(to.buffer, old_to_buffer, to.capacity) == 0);
    }

    /* 7. Assert invariants (validity predicates) */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
