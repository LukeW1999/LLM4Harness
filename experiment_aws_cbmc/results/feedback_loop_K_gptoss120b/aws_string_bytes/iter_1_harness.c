/*  
 * Contract for aws_string_bytes  
 * Preconditions:  
 *   - str is a non‑NULL pointer to a memory region that was allocated as  
 *     sizeof(struct aws_string) + N bytes, where N = str->len + 1 (the extra byte is the null terminator).  
 *   - str->len is a size_t value (any value).  
 *   - The byte array str->bytes[0 .. str->len] is allocated and accessible.  
 *   - No other requirements on str->allocator (may be NULL or any pointer).  
 *   
 * Postconditions (validity):  
 *   - The function returns a non‑NULL pointer equal to str->bytes.  
 *   
 * Postconditions (frame):  
 *   - The call does not modify any memory reachable from str (allocator, len, bytes).  
 *   - No other memory is modified.  
 */  

#include <aws/common/string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers */
static uint8_t nondet_uint8_t(void);
static struct aws_allocator *nondet_allocator(void);

void aws_string_bytes_harness(void) {
    /* Choose a nondeterministic length (bounded to keep the model finite). */
    size_t len;
    __CPROVER_assume(len <= 256);

    /* Allocate memory for the aws_string structure plus the byte array. */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialise fields. */
    str->allocator = nondet_allocator();          /* allocator may be NULL or any pointer */
    str->len = len;

    /* Initialise the byte array (including the mandatory null terminator). */
    for (size_t i = 0; i < len + 1; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }
    ((uint8_t *)str->bytes)[len] = 0; /* explicit null terminator */

    /* Save a snapshot of the observable state for later frame checks. */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t *old_bytes = malloc(len + 1);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < len + 1; ++i) {
        old_bytes[i] = ((uint8_t *)str->bytes)[i];
    }

    /* Call the function under test. */
    const uint8_t *result = aws_string_bytes(str);

    /* Postcondition checks. */
    assert(result != NULL);
    assert(result == str->bytes);

    /* Frame checks – ensure nothing was modified. */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    for (size_t i = 0; i < len + 1; ++i) {
        assert(((uint8_t *)str->bytes)[i] == old_bytes[i]);
    }

    /* Clean up. */
    free((void *)old_bytes);
    free(str);
    return 0;
}
