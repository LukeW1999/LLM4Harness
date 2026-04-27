/* From: include/aws/common/byte_buf.h */

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

/**
 * Copies from->len bytes from from->ptr into to->buffer starting at to->len.
 * Requires: to is a valid aws_byte_buf (aws_byte_buf_is_valid(to)).
 * Requires: from is a valid aws_byte_cursor (aws_byte_cursor_is_valid(from)).
 * Ensures on success (AWS_OP_SUCCESS):
 *   to->len == old_to->len + from->len
 *   to->capacity unchanged
 *   to->allocator unchanged
 *   from is unchanged (ptr, len)
 * Ensures on failure (AWS_OP_ERR, AWS_ERROR_DEST_COPY_TOO_SMALL):
 *   to is unchanged (len, buffer contents, capacity, allocator)
 *   from is unchanged
 * Ensures: to remains valid (aws_byte_buf_is_valid(to))
 * Ensures: from remains valid (aws_byte_cursor_is_valid(from))
 *
 * AWS_PRECONDITION(aws_byte_buf_is_valid(to));
 * AWS_PRECONDITION(aws_byte_cursor_is_valid(from));
 * AWS_POSTCONDITION(aws_byte_buf_is_valid(to));
 * AWS_POSTCONDITION(aws_byte_cursor_is_valid(from));
 */
int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from);

/* Validity predicates:
 * aws_byte_buf_is_valid(buf): buf != NULL, buf->len <= buf->capacity,
 *   (buf->capacity == 0 || buf->buffer != NULL)
 * aws_byte_cursor_is_valid(cursor): cursor != NULL,
 *   (cursor->len == 0 || cursor->ptr != NULL)
 */
