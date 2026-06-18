#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare nondeterministic length and bound it */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a string object with enough space for the flexible array */
    struct aws_string *s = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
    __CPROVER_assume(s != NULL);

    /* 3. Initialize fields (cast away constness for assignment) */
    s->allocator = aws_default_allocator();
    *((size_t *)&s->len) = len;               /* assign to const member */
    /* bytes are left uninitialized – content is irrelevant for this harness */

    /* 4. Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(s));

    /* 5. Save old immutable state */
    struct aws_string old = *s;   /* copies allocator and len (bytes array not needed) */

    /* 6. Call function under test */
    const uint8_t *ptr = aws_string_bytes(s);

    /* 7. Post‑condition assertions */
    /* returned pointer must be the address of the flexible array member */
    assert(ptr == s->bytes);
    assert(ptr != NULL);

    /* unchanged fields */
    assert(s->allocator == old.allocator);
    assert(s->len == old.len);

    /* validity invariant must still hold */
    assert(aws_string_is_valid(s));
}
