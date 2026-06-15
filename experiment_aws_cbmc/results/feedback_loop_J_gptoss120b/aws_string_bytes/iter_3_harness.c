#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* Allocate memory for the string structure plus its flexible array */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* Non‑deterministic length bounded by MAX_STRING_LEN */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    /* Non‑deterministic allocator: either NULL (static) or the default allocator */
    bool use_null_allocator = nondet_bool();
    str->allocator = use_null_allocator ? NULL : aws_default_allocator();

    /* Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Preserve old immutable fields */
    struct aws_string old = *str;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Post‑conditions */
    assert(bytes == str->bytes);               /* returned pointer is the bytes member */
    assert(str->len == old.len);               /* length unchanged */
    assert(str->allocator == old.allocator);   /* allocator unchanged */
    assert(aws_string_is_valid(str));          /* string remains valid */

    /* Clean up */
    free(str);
}
