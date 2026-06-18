#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    const struct aws_string *str;

    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str->len < SIZE_MAX);
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len + 1));
    __CPROVER_assume(str->bytes[str->len] == 0);

    const struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes = str->bytes;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array(str->bytes, str->len + 1, &old_byte);

    const uint8_t *result = aws_string_bytes(str);

    assert(result == old_bytes);
    assert(result == str->bytes);
    assert(result != NULL);

    assert(str == old_str);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes);

    assert_byte_from_buffer_matches(str->bytes, &old_byte);
    assert(str->bytes[str->len] == 0);

    assert(AWS_MEM_IS_READABLE(result, old_len + 1));
    assert(aws_string_is_valid(str));
}
