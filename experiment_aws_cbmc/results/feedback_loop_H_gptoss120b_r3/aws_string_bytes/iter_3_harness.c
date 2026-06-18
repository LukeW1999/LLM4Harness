#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* 1. Allocate and bound a nondeterministic aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate memory for the struct header plus len bytes of data and a null terminator on the stack */
    uint8_t mem[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    struct aws_string *str = (struct aws_string *)mem;

    /* Initialise the string fields */
    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    /* Initialise the data bytes nondeterministically */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }
    /* Null terminator right after the data */
    ((uint8_t *)str->bytes)[len] = 0;

    /* 2. Assume the string satisfies the validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 3. Save old state (shallow copy of header fields) */
    struct aws_string old = *str;

    /* 4. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 5. Post‑condition assertions */
    assert(result == &str->bytes[0]);
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 6. The string must still be valid after the call */
    assert(aws_string_is_valid(str));
}
