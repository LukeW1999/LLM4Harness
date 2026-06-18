#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LEN 256

void aws_string_destroy_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_LEN);

    uint8_t storage[sizeof(struct aws_string) + MAX_LEN + 1];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    /* Null‑terminate the string as aws_string expects a trailing '\\0' */
    str->bytes[len] = 0;

    aws_string_destroy(str);
}
