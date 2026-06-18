#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness(void) {
    /* nondeterministic length, bounded */
    size_t len = nondet_size_t();
    const size_t MAX_LEN = 256;
    __CPROVER_assume(len <= MAX_LEN);

    /* stack‑allocated buffer large enough for struct + bytes */
    unsigned char buffer[sizeof(struct aws_string) + MAX_LEN];
    struct aws_string *str = (struct aws_string *)buffer;

    /* initialize the string */
    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    /* assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* save immutable fields */
    struct aws_string old = *str;

    /* call function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* post‑conditions */
    assert(bytes == str->bytes);
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert(aws_string_is_valid(str));
}
