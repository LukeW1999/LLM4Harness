#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* Allocate a nondeterministic aws_string object */
    struct aws_string *str = malloc(nondet_size_t());
    __CPROVER_assume(str != NULL);

    /* Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save a copy of the original state for immutability checks */
    struct aws_string old = *str;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Post‑condition: the returned pointer must be the bytes field of the struct */
    assert(bytes == str->bytes);

    /* Unchanged fields: the function must not modify any part of the struct */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    /* The flexible array member `bytes` is not modified; its address is unchanged */
    assert(str->bytes == old.bytes);

    /* The string must remain valid after the call */
    assert(aws_string_is_valid(str));

    free(str);
}
