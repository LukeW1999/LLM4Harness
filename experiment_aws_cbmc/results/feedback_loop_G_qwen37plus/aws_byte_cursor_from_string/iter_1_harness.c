#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src = NULL;
    bool is_null = nondet_bool();
    if (!is_null) {
        src = malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(src != NULL);
        src->allocator = NULL;
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == src->bytes);
        assert(cursor.len == src->len);
    }
    
    assert(aws_byte_cursor_is_valid(&cursor));
}
