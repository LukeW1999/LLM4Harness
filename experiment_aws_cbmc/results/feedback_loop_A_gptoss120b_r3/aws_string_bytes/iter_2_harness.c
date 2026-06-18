#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024);

    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    if (nondet_bool()) {
        str->allocator = NULL;
    } else {
        str->allocator = allocator;
    }

    str->len = len;

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_string old = *str;

    const uint8_t *bytes = aws_string_bytes(str);

    assert(bytes == str->bytes);
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert(aws_string_is_valid(str));

    free(str);
}
