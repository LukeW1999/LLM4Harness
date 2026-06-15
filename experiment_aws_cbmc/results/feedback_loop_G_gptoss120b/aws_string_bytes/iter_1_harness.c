#include <stdlib.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/common.h>

/* Bounding constant for the length of the string data */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 256
#endif

void aws_string_bytes_harness(void) {
    /* 1. Allocate and bound a nondeterministic aws_string */
    struct aws_string *str;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate enough space for the struct header plus the flexible array */
    str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Nondeterministically initialize fields */
    str->allocator = (struct aws_allocator *)nondet_uint8_t(); /* allocator may be NULL or any pointer */
    str->len = len;

    /* The bytes array is nondeterministic; no need to initialize it for this harness */

    /* Assume the string is valid according to the library's invariant */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state before the call */
    struct aws_string old = *str;

    /* 3. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    /* The function must return a pointer to the internal bytes array */
    assert(result == str->bytes);

    /* 5. Assert fields that must remain unchanged */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 6. Assert that the string remains valid after the call */
    assert(aws_string_is_valid(str));
}
