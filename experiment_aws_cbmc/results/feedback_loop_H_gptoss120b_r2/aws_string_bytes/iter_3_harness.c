#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdalign.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* Allocate storage for aws_string with flexible array member */
    alignas(struct aws_string) unsigned char buffer[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *str = (struct aws_string *)buffer;

    /* Nondeterministic length bounded by MAX_STRING_LEN */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    /* Set allocator (any allocator is fine) */
    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state (allocator and length) */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    /* Call function under test */
    const uint8_t *ret = aws_string_bytes(str);

    /* Postconditions */
    assert(ret == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(aws_string_is_valid(str));
}
