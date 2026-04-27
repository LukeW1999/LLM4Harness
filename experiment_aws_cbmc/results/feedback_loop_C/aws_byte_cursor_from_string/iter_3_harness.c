#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src;
    size_t len;
    uint8_t *bytes;

    // Allocate and initialize src
    __CPROVER_assume(len > 0 && len <= MAX_MALLOC_SIZE);
    bytes = malloc(len);
    __CPROVER_assume(bytes != NULL);
    src = malloc(sizeof(struct aws_string));
    __CPROVER_assume(src != NULL);
    src->allocator = aws_default_allocator();
    src->len = len;
    src->bytes = bytes;

    // Assume src is valid
    __CPROVER_assume(aws_string_is_valid(src));

    struct aws_string old_src = *src;

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    // Check frame conditions for src
    assert(src->allocator == old_src.allocator);
    assert(src->len == old_src.len);
    assert(src->bytes == old_src.bytes);

    // Check success path
    assert(result.ptr == src->bytes);
    assert(result.len == src->len);

    free(bytes);
    free(src);
}
