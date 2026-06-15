#include <aws/common/byte_buf.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_from_c_str_harness(void) {
    char *c_str;

    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < len; ++i) {
            c_str[i] = nondet_uint8_t();
        }
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
