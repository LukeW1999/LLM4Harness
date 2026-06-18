#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* Allocate a nondeterministic length for the string */
    size_t len = nondet_size_t();

    /* Allocate memory for the aws_string struct plus its flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialize fields (allocator can be nondet or NULL) */
    str->allocator = NULL;
    str->len = len;

    /* Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save a copy of the original immutable fields */
    struct aws_string old = *str;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Post‑condition: the returned pointer must be the bytes field of the struct */
    assert(bytes == str->bytes);

    /* Unchanged fields: the function must not modify any part of the struct */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* The string must remain valid after the call */
    assert(aws_string_is_valid(str));

    free(str);
}
