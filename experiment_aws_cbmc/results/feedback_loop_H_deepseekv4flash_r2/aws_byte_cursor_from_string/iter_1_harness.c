#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src;
    bool is_null = nondet_bool();
    if (is_null) {
        src = NULL;
    } else {
        src = (struct aws_string *)malloc(sizeof(struct aws_string) + sizeof(uint8_t) * (nondet_size_t())); // Approximate allocation
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(src));
        __CPROVER_assume(AWS_MEM_IS_READABLE(aws_string_bytes(src), src->len));
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        // Validity of the input string is preserved (trivially, since it is not modified)
        assert(aws_string_is_valid(src));
    }

    // The returned cursor must satisfy its validity predicate
    assert(aws_byte_cursor_is_valid(&cursor));
}
