#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* Allocate a nondeterministic aws_string with a flexible array */
    size_t len = nondet_size_t();
    /* Bound the length to keep the state space manageable */
    __CPROVER_assume(len <= 1024);

    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Nondeterministically set allocator (NULL or default) */
    if (nondet_bool()) {
        str->allocator = NULL;
    } else {
        str->allocator = aws_default_allocator();
    }

    str->len = len;

    /* Assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save the old immutable fields for later comparison */
    struct aws_string old = *str;

    /* Call the function under test */
    const uint8_t *bytes = aws_string_bytes(str);

    /* Post‑conditions */
    assert(bytes == str->bytes);                 /* returned pointer equals the internal bytes */
    assert(str->allocator == old.allocator);     /* allocator unchanged */
    assert(str->len == old.len);                 /* length unchanged */

    /* The string must remain valid after the call */
    assert(aws_string_is_valid(str));

    free(str);
}
