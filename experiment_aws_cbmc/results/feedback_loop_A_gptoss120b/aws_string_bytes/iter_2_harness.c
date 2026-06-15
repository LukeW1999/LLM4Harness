#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

#define MAX_STRING_SIZE 256

void aws_string_bytes_harness(void) {
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE + 1);
    __CPROVER_assume(str != NULL);

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_SIZE);

    *((struct aws_allocator **)&str->allocator) = NULL;
    *((size_t *)&str->len) = len;

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_string old = *str;

    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len + 1, &old_bytes);

    const uint8_t *ret = aws_string_bytes(str);

    assert(ret == str->bytes);
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert_bytes_match(str->bytes, old.bytes, len + 1);
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    assert(aws_string_is_valid(str));
}
