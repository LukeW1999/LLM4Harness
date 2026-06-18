#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* 1. Allocate and initialize a nondeterministic aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct aws_string *str = malloc(sizeof(struct aws_string) + len * sizeof(uint8_t));
    __CPROVER_assume(str != NULL);

    str->allocator = aws_default_allocator();
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t old_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < len; ++i) {
        old_bytes[i] = str->bytes[i];
    }

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Postconditions */
    /* The returned pointer must be the address of the bytes field */
    assert(result == str->bytes);

    /* Unchanged fields */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    for (size_t i = 0; i < len; ++i) {
        assert(str->bytes[i] == old_bytes[i]);
    }

    /* 5. Validity invariant must still hold */
    assert(aws_string_is_valid(str));

    free(str);
}
