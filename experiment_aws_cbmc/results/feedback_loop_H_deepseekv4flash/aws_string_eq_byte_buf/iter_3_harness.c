#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic allocation and bounding for str */
    struct aws_string *str = can_fail_malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    if (str) {
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len));
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministic allocation and bounding for buf */
    struct aws_byte_buf *buf = can_fail_malloc(sizeof(struct aws_byte_buf));
    if (buf) {
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Assert postconditions */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* Both are non-null */
        if (str->len != buf->len) {
            assert(result == false);
        } else {
            /* Lengths equal, compare bytes */
            bool bytes_equal = true;
            for (size_t i = 0; i < str->len; i++) {
                if (str->bytes[i] != buf->buffer[i]) {
                    bytes_equal = false;
                    break;
                }
            }
            assert(result == bytes_equal);
        }
    }

    /* Unchanged fields */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
