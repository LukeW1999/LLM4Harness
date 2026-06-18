#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LEN 256

void aws_string_bytes_harness(void) {
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    unsigned char buffer[sizeof(struct aws_string) + MAX_LEN];
    struct aws_string *str = (struct aws_string *)buffer;

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    str->allocator = allocator;
    str->len = len;

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *bytes = aws_string_bytes(str);

    assert(bytes == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->allocator != NULL);
    assert(str->len <= MAX_LEN);
}
