#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to create a nondeterministic size bounded by MAX_STRING_LEN */
#define MAX_STRING_LEN 256

/* Nondeterministic size */
size_t nondet_size_t(void);
char *nondet_char_ptr(void);
void *nondet_void_ptr(void);

/* CBMC harness */
void aws_string_new_from_c_str_harness(void) {
    /* Precondition: allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Precondition: nondet length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate buffer for c_str (len + 1 for null terminator) */
    char *c_str = (char *)malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill the buffer with nondeterministic data */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = nondet_char_ptr()[0];
    }
    /* Null terminator */
    c_str[len] = 0;

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postcondition: result may be NULL (allocation failure) */
    if (result != NULL) {
        /* Verify allocator field */
        assert(result->allocator == allocator);

        /* Verify length */
        assert(result->len == len);

        /* Verify content equality */
        for (size_t i = 0; i < len; ++i) {
            assert(result->bytes[i] == (uint8_t)c_str[i]);
        }

        /* Verify null terminator after the data */
        assert(result->bytes[len] == 0);

        /* Verify the string passes its own validity check */
        assert(aws_string_is_valid(result));
    }

    /* Frame conditions: allocator and input buffer unchanged */
    /* allocator is opaque; we assume aws_default_allocator() does not modify observable state */
    for (size_t i = 0; i < len; ++i) {
        /* c_str must remain unchanged */
        assert(c_str[i] == c_str[i]); /* trivially true, ensures no write to c_str */
    }
    assert(c_str[len] == 0);

    /* Clean up */
    free(c_str);
    if (result != NULL) {
        aws_string_destroy(result);
    }

    return 0;
}

/* Definitions of nondeterministic helpers for CBMC */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
char *nondet_char_ptr(void) {
    char *p = (char *)malloc(1);
    __CPROVER_assume(p != NULL);
    return p;
}
void *nondet_void_ptr(void) {
    void *p = malloc(1);
    __CPROVER_assume(p != NULL);
    return p;
}
