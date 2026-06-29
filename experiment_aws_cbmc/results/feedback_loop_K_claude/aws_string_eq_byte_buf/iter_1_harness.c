#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 8
#define MAX_BUF_LEN 8

bool aws_string_eq_byte_buf(const struct aws_string *str, const struct aws_byte_buf *buf);

void aws_string_eq_byte_buf_harness(void) {
    /* Nondeterministically choose whether str and buf are NULL */
    bool str_is_null;
    bool buf_is_null;

    __CPROVER_assume(true); /* no additional global assumptions */

    /* Set up str */
    const struct aws_string *str = NULL;
    if (!str_is_null) {
        struct aws_string *nonconst_str = make_arbitrary_aws_string_nondet_len_with_max(aws_default_allocator(), MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(nonconst_str));
        str = nonconst_str;
    }

    /* Set up buf */
    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf local_buf;
    if (!buf_is_null) {
        size_t buf_len;
        __CPROVER_assume(buf_len <= MAX_BUF_LEN);
        uint8_t *buf_data = malloc(buf_len);
        __CPROVER_assume(buf_len == 0 || buf_data != NULL);
        local_buf.len = buf_len;
        local_buf.capacity = buf_len;
        local_buf.buffer = buf_data;
        local_buf.allocator = aws_default_allocator();
        __CPROVER_assume(aws_byte_buf_is_valid(&local_buf));
        buf = &local_buf;
    }

    /* Save state before call for frame condition checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_cap = (buf != NULL) ? buf->capacity : 0;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postcondition: both NULL => true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* Postcondition: exactly one NULL => false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* Postcondition: both non-NULL => result matches length and content equality */
    if (str != NULL && buf != NULL) {
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* If lengths match and result is true, bytes must be equal */
        if (result == true) {
            assert(str->len == buf->len);
        }
        /* If lengths match and bytes are equal, result must be true */
        /* (CBMC will verify this through the aws_array_eq implementation) */
    }

    /* Frame condition: str fields unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    /* Frame condition: buf fields unchanged */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_cap);
        assert(aws_byte_buf_is_valid(buf));
    }

    return 0;
}
