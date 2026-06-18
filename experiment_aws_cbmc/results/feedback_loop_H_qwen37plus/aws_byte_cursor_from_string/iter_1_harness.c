#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src = NULL;
    bool is_null = nondet_bool();
    
    if (!is_null) {
        src = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(src != NULL);
        src->allocator = aws_default_allocator();
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
    }
    
    assert(aws_byte_cursor_is_valid(&result));
}
