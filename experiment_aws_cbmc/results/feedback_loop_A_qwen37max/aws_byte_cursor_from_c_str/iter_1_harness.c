#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness() {
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    
    bool is_null = nondet_bool();
    char *c_str = NULL;
    if (!is_null) {
        c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[str_len] = '\0';
        for (size_t i = 0; i < str_len; i++) {
            c_str[i] = nondet_bool() ? 'a' : 'b';
        }
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == str_len);
    }
    
    assert(aws_byte_cursor_is_valid(&cur));
}
