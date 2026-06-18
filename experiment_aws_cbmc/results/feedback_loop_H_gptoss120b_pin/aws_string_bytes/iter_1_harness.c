#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 256

void aws_string_bytes_harness(void) {
    /* Allocate a string with space for a bounded number of bytes */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
    __CPROVER_assume(str != NULL);

    /* Non‑deterministic length bounded by the allocated buffer */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_SIZE);

    /* Initialise fields (const members are set via casts) */
    *((struct aws_allocator **)&str->allocator) = aws_default_allocator();
    *((size_t *)&str->len) = len;

    /* Assume the string satisfies the library’s validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save immutable fields for later comparison */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Post‑conditions */
    /* The returned pointer must be the address of the internal byte array */
    assert(bytes == str->bytes);

    /* No fields of the string are modified */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* The string remains valid after the call */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(str);
}
