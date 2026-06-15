#include <aws/common/string.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

/* Helper bounds – adjust as needed for the verification */
#define MAX_STR_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* 1. Non‑deterministic inputs */
    struct aws_allocator *allocator = (struct aws_allocator *)nondet_uint8_t(); /* allocator pointer can be NULL */
    char c_str_buf[MAX_STR_LEN];
    size_t str_len = nondet_size_t();

    /* 2. Constrain the length to be within bounds and ensure null‑termination */
    __CPROVER_assume(str_len < MAX_STR_LEN);
    for (size_t i = 0; i < str_len; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    c_str_buf[str_len] = '\0';               /* guarantee a terminating NUL */
    const char *c_str = c_str_buf;

    /* 3. Save old state of inputs (allocator is immutable, c_str buffer is read‑only) */
    struct aws_allocator *old_allocator = allocator;

    /* 4. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 5. Post‑condition checks */
    if (result != NULL) {
        /* Success path – the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The allocator stored in the string must be the same as the input allocator */
        assert(result->allocator == allocator);
        assert(allocator != NULL);               /* allocation succeeded, so allocator cannot be NULL */

        /* Length must equal the length of the input C‑string (excluding the NUL) */
        assert(result->len == str_len);
        assert(result->len == strlen(c_str));

        /* The bytes stored in the string must match the input bytes */
        for (size_t i = 0; i < result->len; ++i) {
            assert(result->bytes[i] == (uint8_t)c_str[i]);
        }

        /* The byte after the data must be a NUL terminator (guaranteed by implementation) */
        assert(result->bytes[result->len] == 0);
    } else {
        /* Failure path – no string was allocated, so nothing changes */
        /* The allocator argument is unchanged (it is an input pointer) */
        assert(allocator == old_allocator);
        /* No additional state to check on failure */
    }

    /* 6. Invariant: the allocator pointer itself is unchanged regardless of outcome */
    assert(allocator == old_allocator);
}
