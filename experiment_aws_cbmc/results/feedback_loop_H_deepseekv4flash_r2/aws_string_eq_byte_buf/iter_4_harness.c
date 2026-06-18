#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* Non-deterministic allocation of string */
    if (nondet_bool()) {
        size_t len;
        __CPROVER_assume(len < MAX_STRING_LEN);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        for (size_t i = 0; i < len; i++) {
            str->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministic allocation of byte buffer */
    if (nondet_bool()) {
        buf = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    }

    /* Validity invariants still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
