#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    uint64_t nondet_len = nondet_uint64_t();
    size_t len = (size_t)nondet_len;
    __CPROVER_assume(len <= 1024);

    uint8_t storage[sizeof(struct aws_string) + 1024];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    if (nondet_bool()) {
        str->allocator = NULL;
    } else {
        str->allocator = allocator;
    }

    str->len = len;

    /* Ensure the flexible array has space for a terminating NUL byte */
    if (len < 1024) {
        str->bytes[len] = 0;
    }

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *bytes = aws_string_bytes(str);

    assert(bytes == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
}
