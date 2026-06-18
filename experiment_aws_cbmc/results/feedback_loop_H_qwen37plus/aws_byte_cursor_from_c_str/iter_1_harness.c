#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness(void) {
    const char *c_str = NULL;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_len = 1024;
        c_str = malloc(max_len);
        __CPROVER_assume(c_str != NULL);
        size_t len = nondet_size_t();
        __CPROVER_assume(len < max_len);
        c_str[len] = '\0';
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    }

    assert(aws_byte_cursor_is_valid(&cur));
}
