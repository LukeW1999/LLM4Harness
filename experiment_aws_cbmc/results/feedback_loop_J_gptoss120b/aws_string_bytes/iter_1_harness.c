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
    /* Allocate a string with enough space for the flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* Non‑deterministic length bounded by MAX_STRING_LEN */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    /* Allocator can be NULL (static string) or a valid allocator.
       For the purpose of the harness we allow any value and rely on the
       validity predicate to filter out invalid cases. */
    str->allocator = nondet_bool() ? NULL : aws_default_allocator();

    /* Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old immutable fields */
    struct aws_string old = *str;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Post‑conditions */
    /* The returned pointer must be the address of the bytes member */
    assert(bytes == str->bytes);

    /* No fields of the string may have changed */
    assert(str->len == old.len);
    assert(str->allocator == old.allocator);

    /* The string must remain valid */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(str);
}
