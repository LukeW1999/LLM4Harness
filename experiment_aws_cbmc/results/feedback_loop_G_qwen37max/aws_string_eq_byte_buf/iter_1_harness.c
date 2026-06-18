#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = nondet_bool() ? NULL : malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
    }

    struct aws_byte_buf *buf = nondet_bool() ? NULL : malloc(sizeof(struct aws_byte_buf));
    if (buf != NULL) {
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
        if (result) {
            assert(str->len == buf->len);
            assert_bytes_match(str->bytes, buf->buffer, str->len);
        }
    }
}
