#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;
    
    struct aws_string old_str;
    struct aws_byte_buf old_buf;
    
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        __CPROVER_assume(aws_string_is_valid(str));
        old_str = *str;
    }
    
    if (nondet_bool()) {
        buf_ptr = &buf;
        __CPROVER_assume(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf_ptr);
        __CPROVER_assume(aws_byte_buf_is_valid(buf_ptr));
        old_buf = *buf_ptr;
    }
    
    bool result = aws_string_eq_byte_buf(str, buf_ptr);
    (void)result;
    
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(aws_string_is_valid(str));
    }
    
    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->buffer == old_buf.buffer);
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}
