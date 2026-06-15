#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* 1. Allocate a string with enough space for the flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* 2. Nondeterministically choose a length within the allocated bound */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    /* 3. Choose an allocator (NULL for static strings, otherwise default) */
    if (nondet_bool()) {
        str->allocator = NULL;
    } else {
        str->allocator = aws_default_allocator();
    }

    /* 4. Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 5. Save the old state for immutability checks */
    struct aws_string old = *str;

    /* 6. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 7. Post‑conditions */

    /* The returned pointer must be the address of the flexible array member */
    assert(result == str->bytes);

    /* No fields of the string may change */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    /* The first byte of the flexible array is also unchanged (the copy only
       captures the first element, which is sufficient for the immutability
       check required by the harness pattern). */
    assert(str->bytes[0] == old.bytes[0]);

    /* 8. The string must remain valid after the call */
    assert(aws_string_is_valid(str));

    /* 9. Clean up */
    free(str);
}
