#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN MAX_BUFFER_SIZE

void aws_string_bytes_harness(void) {
    /* 1. Allocate and bound a nondeterministic aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* allocate space for the struct header plus len bytes of data and a null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    str->allocator = aws_default_allocator();
    str->len = len;

    /* initialise the data bytes nondeterministically */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }
    /* null terminator right after the data */
    ((uint8_t *)str->bytes)[len] = 0;

    /* 2. Assume the string satisfies the validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 3. Save old state (shallow copy of header fields) */
    struct aws_string old = *str;

    /* 4. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 5. Post‑condition assertions */
    /* result must point to the first byte of the string's data */
    assert(result == &str->bytes[0]);

    /* fields that must remain unchanged */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 6. The string must still be valid after the call */
    assert(aws_string_is_valid(str));
}
