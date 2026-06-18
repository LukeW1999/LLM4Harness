/*  
Preconditions:  
- __CPROVER_assume(buf != NULL);  
- __CPROVER_assume(c_str != NULL || buf->len == 0);  // c_str may be NULL only when buf length is zero  
- __CPROVER_assume(buf->capacity >= buf->len);  
- __CPROVER_assume(buf->allocator == aws_default_allocator());  
- __CPROVER_assume(aws_byte_buf_is_valid(buf));  // validates internal invariants (buffer may be NULL only if len == 0)  
- __CPROVER_assume(buf->buffer != NULL || buf->len == 0);  
- __CPROVER_assume(buf->len <= buf->capacity);  
- __CPROVER_assume(buf->capacity <= MAX_ALLOC_SIZE); // arbitrary bound for CBMC  
- __CPROVER_assume(c_str_len >= 0 && c_str_len <= MAX_STRING_LEN); // length of c_str without the terminating '\0'  
- __CPROVER_assume(c_str != NULL ==> c_str[c_str_len] == '\0'); // null‑terminated  

Postconditions (validity):  
- The function returns a bool equal to the result of aws_array_eq_c_str(buf->buffer, buf->len, c_str).  

Postconditions (length / shape):  
- The aws_byte_buf *buf is not modified: len, capacity, allocator, and buffer pointer remain unchanged.  
- The contents of buf->buffer (up to buf->len) are unchanged.  
- The contents of c_str (including the terminating '\0') are unchanged.  

Postconditions (frame):  
- No memory outside of buf, buf->buffer, and c_str is modified.  
*/

#include <aws/common/byte_buf.h>
#include <aws/common/array.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_ALLOC_SIZE 1024
#define MAX_STRING_LEN 256

void aws_byte_buf_eq_c_str_harness(void) {
    /* Allocate and nondet‑initialize the aws_byte_buf */
    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();

    /* Nondet capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_ALLOC_SIZE);
    buf.capacity = capacity;

    /* Allocate buffer memory (may be NULL if capacity == 0) */
    if (capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* Nondet contents */
        for (size_t i = 0; i < capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    } else {
        buf.buffer = NULL;
    }

    /* Nondet length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    /* Ensure the byte buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Allocate and nondet‑initialize the C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_STRING_LEN);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* Save copies of the original state for later comparison */
    struct aws_byte_buf buf_old = buf;
    uint8_t *buf_contents_old = NULL;
    if (buf.buffer != NULL && buf.len > 0) {
        buf_contents_old = malloc(buf.len);
        __CPROVER_assume(buf_contents_old != NULL);
        memcpy(buf_contents_old, buf.buffer, buf.len);
    }
    char *c_str_old = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_old != NULL);
    memcpy(c_str_old, c_str, c_str_len + 1);

    /* Call the function under verification */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postcondition: result matches the specification */
    bool expected = aws_array_eq_c_str(buf.buffer, buf.len, c_str);
    assert(result == expected);

    /* Postcondition: buf structure unchanged */
    assert(buf.len == buf_old.len);
    assert(buf.capacity == buf_old.capacity);
    assert(buf.allocator == buf_old.allocator);
    assert(buf.buffer == buf_old.buffer);

    /* Postcondition: buffer contents unchanged */
    if (buf.buffer != NULL && buf.len > 0) {
        assert(memcmp(buf.buffer, buf_contents_old, buf.len) == 0);
    }

    /* Postcondition: c_str unchanged */
    assert(memcmp(c_str, c_str_old, c_str_len + 1) == 0);

    /* Clean up */
    if (buf.buffer) free(buf.buffer);
    if (buf_contents_old) free(buf_contents_old);
    free(c_str);
    free(c_str_old);

    return 0;
}
