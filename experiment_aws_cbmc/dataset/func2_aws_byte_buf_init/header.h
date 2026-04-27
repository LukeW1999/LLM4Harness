/* From: include/aws/common/byte_buf.h */

/**
 * Initializes an aws_byte_buf structure with a new backing buffer.
 * Requires: buf and allocator are non-null.
 * Ensures: buf is a valid aws_byte_buf with capacity bytes allocated.
 *          buf->len == 0, buf->allocator == allocator.
 *          Returns AWS_OP_SUCCESS on success, AWS_OP_ERR on failure.
 */
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
