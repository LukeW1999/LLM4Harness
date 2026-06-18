#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    char *c_str = NULL;
    if (nondet_bool()) {
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < len; ++i) {
            __CPROVER_assume(c_str[i] != '\0');
        }
        c_str[len] = '\0';
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    assert(cur.ptr == (uint8_t *)c_str);
    if (c_str != NULL) {
        assert(cur.len == len);
    } else {
        assert(cur.len == 0);
    }
    
    assert(aws_byte_cursor_is_valid(&cur));
}
