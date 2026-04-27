/* From: include/aws/common/byte_buf.h */


AWS_COMMON_API
int aws_byte_buf_init(struct aws_byte_buf *buf, struct aws_allocator *allocator, size_t capacity);

/* Validity predicate (used in pre/postconditions): */
/* bool aws_byte_buf_is_valid(const struct aws_byte_buf *buf); */
/* Checks: buf != NULL, (buf->len <= buf->capacity), */
/*         (buf->capacity == 0 || buf->buffer != NULL). */

struct aws_byte_buf {
    size_t len;
    uint8_t *buffer;
    size_t capacity;
    struct aws_allocator *allocator;
};
