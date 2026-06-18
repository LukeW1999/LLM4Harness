#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare and bound the aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Stack‑allocated wrapper with enough space for the flexible array */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } wrapper;

    struct aws_string *str = &wrapper.s;

    /* Initialize members */
    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    /* Nondeterministically initialize the bytes */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    /* 2. Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len, &old_bytes);

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    assert(result == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    assert(aws_string_is_valid(str));
}
