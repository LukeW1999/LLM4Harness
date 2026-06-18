#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    if (!str_is_null) {
        str = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        str->bytes = (const uint8_t *)(str + 1);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        if (result) {
            assert(str->len == cur->len);
            assert_bytes_match(str->bytes, cur->ptr, str->len);
        }
        assert(aws_string_is_valid(str));
        assert(aws_byte_cursor_is_valid(cur));
    }
}
