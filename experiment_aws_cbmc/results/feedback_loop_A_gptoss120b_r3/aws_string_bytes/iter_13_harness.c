#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

void aws_string_bytes_harness(void) {
    size_t len;
    __CPROVER_assume(len <= 1024);

    uint8_t storage[sizeof(struct aws_string) + 1024];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    if (len < 1024) {
        str->bytes[len] = 0;
    }

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *bytes = aws_string_bytes(str);

    __CPROVER_assert(bytes == str->bytes, "bytes points to internal storage");
    __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old_len, "len unchanged");
}
