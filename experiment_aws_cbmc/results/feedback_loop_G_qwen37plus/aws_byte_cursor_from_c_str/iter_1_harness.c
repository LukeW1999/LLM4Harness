#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[len] = '\0';
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (const uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    }

    assert(aws_byte_cursor_is_valid(&cur));
    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
}
