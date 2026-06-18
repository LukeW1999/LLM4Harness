/*=== CONTRACT ===
Preconditions:
  - a != NULL && b != NULL
  - a->len <= MAX_CURSOR_LEN
  - b->len <= MAX_CURSOR_LEN
  - (a->len == 0) ==> a->ptr == NULL || a->ptr can be any value
  - (a->len > 0)  ==> a->ptr != NULL && a->ptr points to a readable region of at least a->len bytes
  - (b->len == 0) ==> b->ptr == NULL || b->ptr can be any value
  - (b->len > 0)  ==> b->ptr != NULL && b->ptr points to a readable region of at least b->len bytes

Postconditions (validity):
  - The function returns a bool equal to the result of aws_array_eq on the two buffers.
  - No memory is modified: a, b, a->ptr[0..a->len-1], b->ptr[0..b->len-1] remain unchanged.

Postconditions (length):
  - a->len and b->len are unchanged.
  - a->ptr and b->ptr are unchanged.

Postconditions (frame):
  - No other memory locations are written by aws_byte_cursor_eq.
=== END CONTRACT ===*/

#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CURSOR_LEN 256

void aws_byte_cursor_eq_harness(void) {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* nondet lengths */
    a.len = nondet_size_t();
    b.len = nondet_size_t();

    __CPROVER_assume(a.len <= MAX_CURSOR_LEN);
    __CPROVER_assume(b.len <= MAX_CURSOR_LEN);

    /* allocate buffers according to length */
    if (a.len > 0) {
        a.ptr = malloc(a.len);
        __CPROVER_assume(a.ptr != NULL);
        for (size_t i = 0; i < a.len; ++i) {
            a.ptr[i] = nondet_uint8_t();
        }
    } else {
        a.ptr = NULL;
    }

    if (b.len > 0) {
        b.ptr = malloc(b.len);
        __CPROVER_assume(b.ptr != NULL);
        for (size_t i = 0; i < b.len; ++i) {
            b.ptr[i] = nondet_uint8_t();
        }
    } else {
        b.ptr = NULL;
    }

    /* Preserve copies of the original buffers and metadata */
    uint8_t *a_copy = NULL;
    uint8_t *b_copy = NULL;
    size_t a_len_orig = a.len;
    size_t b_len_orig = b.len;
    uint8_t *a_ptr_orig = a.ptr;
    uint8_t *b_ptr_orig = b.ptr;

    if (a.len > 0) {
        a_copy = malloc(a.len);
        __CPROVER_assume(a_copy != NULL);
        memcpy(a_copy, a.ptr, a.len);
    }
    if (b.len > 0) {
        b_copy = malloc(b.len);
        __CPROVER_assume(b_copy != NULL);
        memcpy(b_copy, b.ptr, b.len);
    }

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Expected result using the specification */
    bool expected = aws_array_eq(a.ptr, a.len, b.ptr, b.len);
    assert(result == expected);

    /* Verify that the inputs were not modified */
    if (a.len > 0) {
        assert(memcmp(a.ptr, a_copy, a.len) == 0);
    }
    if (b.len > 0) {
        assert(memcmp(b.ptr, b_copy, b.len) == 0);
    }

    /* Verify that lengths and pointers themselves are unchanged */
    assert(a.len == a_len_orig);
    assert(b.len == b_len_orig);
    assert(a.ptr == a_ptr_orig);
    assert(b.ptr == b_ptr_orig);

    /* Clean up */
    if (a.ptr) free(a.ptr);
    if (b.ptr) free(b.ptr);
    if (a_copy) free(a_copy);
    if (b_copy) free(b_copy);
}
