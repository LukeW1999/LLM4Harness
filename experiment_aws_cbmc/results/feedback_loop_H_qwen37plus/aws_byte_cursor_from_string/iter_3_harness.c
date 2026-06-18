#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src = NULL;
    bool is_null = nondet_bool();
    
    if (!is_null) {
        src = ensure_string_is_allocated(MAX_STRING_SIZE);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        assert((const uint8_t *)result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
    }
    
    assert(aws_byte_cursor_is_valid(&result));
}
