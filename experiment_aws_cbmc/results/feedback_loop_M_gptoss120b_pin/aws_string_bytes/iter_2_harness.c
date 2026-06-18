#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare nondeterministic length and bound it */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate storage for the string object on the stack */
    uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *s = (struct aws_string *)storage;

    /* 3. Initialize fields */
    s->allocator = aws_default_allocator();
    s->len = len; /* bytes are left uninitialized – content is irrelevant */

    /* 4. Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(s));

    /* 5. Save old immutable state */
    struct aws_string old = *s;

    /* 6. Call function under test */
    const uint8_t *ptr = aws_string_bytes(s);

    /* 7. Post‑condition assertions */
    assert(ptr == s->bytes);
    assert(ptr != NULL);
    assert(s->allocator == old.allocator);
    assert(s->len == old.len);
    assert(aws_string_is_valid(s));
}
