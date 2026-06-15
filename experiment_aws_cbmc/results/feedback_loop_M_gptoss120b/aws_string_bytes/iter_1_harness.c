#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Harness for aws_string_bytes */
void aws_string_bytes_harness(void) {
    /* 1. Allocate a nondeterministic aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);               /* bound length */

    /* allocate enough memory for the struct + (len) extra bytes (struct already has 1 byte) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);                         /* non‑null pointer */

    /* 2. Initialise the (const) fields via casts – the struct is immutable after creation */
    *((struct aws_allocator **)&str->allocator) = aws_default_allocator();
    *((size_t *)&str->len) = len;

    /* 3. Initialise the byte payload (including the required null terminator) */
    uint8_t *bytes = (uint8_t *)str->bytes;
    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[len] = 0;                                         /* null terminator */

    /* 4. Assume the string is valid before the call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 5. Save old immutable state for later comparison */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t *old_bytes = malloc(old_len + 1);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i <= old_len; ++i) {
        old_bytes[i] = bytes[i];
    }

    /* 6. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 7. Post‑condition checks */

    /* The returned pointer must be the address of the internal byte array */
    assert(result == str->bytes);

    /* No fields of the string may have changed */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* The byte contents must be unchanged */
    for (size_t i = 0; i <= old_len; ++i) {
        assert(result[i] == old_bytes[i]);
    }

    /* The string must still satisfy its validity predicate */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(old_bytes);
    free(str);
}
