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


int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from);

/* Validity predicates:
 * aws_byte_buf_is_valid(buf): buf != NULL, buf->len <= buf->capacity,
 *   (buf->capacity == 0 || buf->buffer != NULL)
 * aws_byte_cursor_is_valid(cursor): cursor != NULL,
 *   (cursor->len == 0 || cursor->ptr != NULL)
 */
