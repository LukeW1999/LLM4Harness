#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_buf buf_obj;
    struct aws_byte_buf *buf = NULL;
    if (!buf_is_null) {
        buf = &buf_obj;
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    struct aws_string old_str = str ? *str : (struct aws_string){0};
    struct aws_byte_buf old_buf = buf ? *buf : (struct aws_byte_buf){0};

    bool result = aws_string_eq_byte_buf(str, buf);

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(aws_string_is_valid(str));
    }
    
    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf));
    }
}
