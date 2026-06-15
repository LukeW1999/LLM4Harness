#include <stdlib.h>
#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_SIZE 256

void aws_string_bytes_harness(void) {
    /* allocate a string with space for a bounded number of bytes */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_SIZE);
    __CPROVER_assume(str != NULL);

    /* nondeterministically choose a length within the bound */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_SIZE);

    /* initialise fields (const fields can be set via a cast for the harness) */
    *((struct aws_allocator **)&str->allocator) = NULL;
    *((size_t *)&str->len) = len;

    /* assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* save a copy of the whole struct for later unchanged‑field checks */
    struct aws_string old = *str;

    /* save a snapshot of the byte array contents */
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len + 1, &old_bytes);

    /* call the function under test */
    const uint8_t *ret = aws_string_bytes(str);

    /* post‑condition: returned pointer is the address of the bytes member */
    assert(ret == str->bytes);

    /* unchanged fields */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* the byte array must be unchanged */
    assert_bytes_match(str->bytes, old.bytes, len + 1);
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);

    /* validity invariant must still hold */
    assert(aws_string_is_valid(str));
}
