/*  
Contract for aws_byte_cursor_eq_byte_buf  

Preconditions:  
- a != NULL && b != NULL  
- a->len <= SIZE_MAX and b->len <= SIZE_MAX  
- (a->len == 0) ==> a->ptr == NULL  (or a->ptr may be any value, but not dereferenced)  
- (b->len == 0) ==> b->buffer == NULL (or any value)  
- a->ptr points to a memory region of at least a->len bytes (if a->len > 0)  
- b->buffer points to a memory region of at least b->len bytes (if b->len > 0)  
- aws_byte_cursor_is_valid(a) holds  
- aws_byte_buf_is_valid(b) holds  

Postconditions (validity):  
- The function returns true iff the byte sequences are equal, i.e.  
  result == aws_array_eq(a->ptr, a->len, b->buffer, b->len)  

Postconditions (frame):  
- Neither *a nor *b nor the memory they reference is modified by the call.  
- No allocation or deallocation occurs.  
*/

#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_LEN 256

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* nondeterministic lengths */
    size_t a_len = nondet_size_t();
    size_t b_len = nondet_size_t();

    __CPROVER_assume(a_len <= MAX_LEN);
    __CPROVER_assume(b_len <= MAX_LEN);

    /* allocate memory for cursor and buffer */
    uint8_t *a_ptr = NULL;
    uint8_t *b_buf = NULL;

    if (a_len > 0) {
        a_ptr = malloc(a_len);
        __CPROVER_assume(a_ptr != NULL);
    }

    if (b_len > 0) {
        b_buf = malloc(b_len);
        __CPROVER_assume(b_buf != NULL);
    }

    /* initialize structures */
    struct aws_byte_cursor a = {
        .len = a_len,
        .ptr = a_ptr
    };

    struct aws_byte_buf b = {
        .len = b_len,
        .capacity = b_len,
        .buffer = b_buf,
        .allocator = aws_default_allocator()
    };

    /* assume the validity predicates hold */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* make copies of the inputs to check frame condition */
    struct aws_byte_cursor a_orig = a;
    struct aws_byte_buf b_orig = b;

    uint8_t *a_ptr_copy = NULL;
    uint8_t *b_buf_copy = NULL;

    if (a_len > 0) {
        a_ptr_copy = malloc(a_len);
        __CPROVER_assume(a_ptr_copy != NULL);
        memcpy(a_ptr_copy, a_ptr, a_len);
    }

    if (b_len > 0) {
        b_buf_copy = malloc(b_len);
        __CPROVER_assume(b_buf_copy != NULL);
        memcpy(b_buf_copy, b_buf, b_len);
    }

    /* call the function under verification */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* compute the expected result using the same primitive */
    bool expected = aws_array_eq(a.ptr, a.len, b.buffer, b.len);

    /* postcondition: return value matches expected */
    assert(result == expected);

    /* frame condition: structures themselves unchanged */
    assert(a.len == a_orig.len);
    assert(a.ptr == a_orig.ptr);
    assert(b.len == b_orig.len);
    assert(b.capacity == b_orig.capacity);
    assert(b.buffer == b_orig.buffer);
    assert(b.allocator == b_orig.allocator);

    /* frame condition: pointed-to memory unchanged */
    if (a_len > 0) {
        assert(memcmp(a.ptr, a_ptr_copy, a_len) == 0);
    }
    if (b_len > 0) {
        assert(memcmp(b.buffer, b_buf_copy, b_len) == 0);
    }

    return 0;
}
