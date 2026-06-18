#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_cursor_from_string_harness() {
    struct {
        struct aws_string str;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } src_obj;

    const struct aws_string *src = NULL;
    if (!nondet_bool()) {
        src = &src_obj.str;
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(result.len == 0);
        assert(result.ptr == NULL);
    } else {
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));
    }

    assert(aws_byte_cursor_is_valid(&result));
}
