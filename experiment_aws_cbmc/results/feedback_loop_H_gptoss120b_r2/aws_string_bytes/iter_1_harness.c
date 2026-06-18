#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* 1. Allocate and bound aws_string */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* nondet length bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    /* allocator (static strings have NULL, but any allocator is fine) */
    str->allocator = aws_default_allocator();

    /* bytes are part of the allocation; leave contents nondet */
    /* 2. Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 3. Save old state */
    struct aws_string old = *str; /* copies allocator and len; bytes are flexible */

    /* 4. Call function under test */
    const uint8_t *ret = aws_string_bytes(str);

    /* 5. Postconditions */

    /* Return pointer must be the address of the bytes field */
    assert(ret == str->bytes);

    /* Unchanged fields */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* Validity invariant must still hold */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(str);
}
