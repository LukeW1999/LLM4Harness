#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_mut {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[1];
};

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        struct aws_string_mut *mut_str = (struct aws_string_mut *)str;
        mut_str->allocator = NULL;
        mut_str->len = len;
    }

    struct aws_byte_buf *buf = NULL;
    if (nondet_bool()) {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        if (str->len != buf->len) {
            assert(result == false);
        }
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
