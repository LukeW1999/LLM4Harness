#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    bool is_null = nondet_bool();
    const char *c_str = NULL;
    char c_str_arr[MAX_BUFFER_SIZE + 1];
    
    if (!is_null) {
        c_str_arr[MAX_BUFFER_SIZE] = '\0';
        c_str = c_str_arr;
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
