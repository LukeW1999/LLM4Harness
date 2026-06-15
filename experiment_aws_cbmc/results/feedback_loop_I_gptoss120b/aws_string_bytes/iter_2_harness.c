#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* Allocate a string with space for the flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* Choose a nondeterministic length within the allocated bound */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    /* Choose an allocator: either NULL (static) or the default allocator */
    if (nondet_bool()) {
        str->allocator = NULL;
    } else {
        str->allocator = aws_default_allocator();
    }

    /* Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save the old state for immutability checks */
    struct aws_string old = *str;
    uint8_t old_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < len; ++i) {
        old_bytes[i] = str->bytes[i];
    }

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Post‑conditions */

    /* The returned pointer must be the address of the flexible array member */
    assert(result == str->bytes);

    /* No fields of the string may change */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* The contents of the flexible array must remain unchanged */
    for (size_t i = 0; i < len; ++i) {
        assert(str->bytes[i] == old_bytes[i]);
    }

    /* The string must remain valid after the call */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(str);
}
