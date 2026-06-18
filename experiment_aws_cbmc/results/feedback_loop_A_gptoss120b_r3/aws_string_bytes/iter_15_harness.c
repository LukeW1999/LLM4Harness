#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024);

    struct {
        struct aws_string header;
        uint8_t buf[1024];
    } storage;

    struct aws_string *str = &storage.header;

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
