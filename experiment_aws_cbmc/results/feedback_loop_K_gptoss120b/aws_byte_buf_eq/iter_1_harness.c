/*=== Contract for aws_byte_buf_eq ===
Preconditions:
  - a != NULL
  - b != NULL
  - aws_byte_buf_is_valid(a)
  - aws_byte_buf_is_valid(b)
Postconditions (validity):
  - a and b remain valid (aws_byte_buf_is_valid holds)
Postconditions (length/capacity):
  - a->len, a->capacity, b->len, b->capacity are unchanged
Postconditions (frame):
  - The contents of a->buffer[0..a->len-1] and b->buffer[0..b->len-1] are unchanged
  - No other memory is modified
*/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* Allocate and nondet-initialize first byte buffer */
    struct aws_byte_buf a;
    a.allocator = alloc;
    a.capacity = nondet_size_t();
    __CPROVER_assume(a.capacity <= 1024);               /* bound for CBMC */
    if (a.capacity > 0) {
        a.buffer = aws_mem_acquire(alloc, a.capacity);
        __CPROVER_assume(a.buffer != NULL);
    } else {
        a.buffer = NULL;
    }
    a.len = nondet_size_t();
    __CPROVER_assume(a.len <= a.capacity);
    /* nondet fill buffer contents up to a.len */
    if (a.len > 0) {
        uint8_t *ptr = a.buffer;
        for (size_t i = 0; i < a.len; ++i) {
            ptr[i] = nondet_uint8_t();
        }
    }

    /* Allocate and nondet-initialize second byte buffer */
    struct aws_byte_buf b;
    b.allocator = alloc;
    b.capacity = nondet_size_t();
    __CPROVER_assume(b.capacity <= 1024);
    if (b.capacity > 0) {
        b.buffer = aws_mem_acquire(alloc, b.capacity);
        __CPROVER_assume(b.buffer != NULL);
    } else {
        b.buffer = NULL;
    }
    b.len = nondet_size_t();
    __CPROVER_assume(b.len <= b.capacity);
    if (b.len > 0) {
        uint8_t *ptr = b.buffer;
        for (size_t i = 0; i < b.len; ++i) {
            ptr[i] = nondet_uint8_t();
        }
    }

    /* Assume both buffers are valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save copies for frame condition checks */
    struct aws_byte_buf a_old = a;
    struct aws_byte_buf b_old = b;

    /* Save contents */
    uint8_t a_old_contents[1024];
    uint8_t b_old_contents[1024];
    if (a.len > 0) {
        memcpy(a_old_contents, a.buffer, a.len);
    }
    if (b.len > 0) {
        memcpy(b_old_contents, b.buffer, b.len);
    }

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Postcondition: result matches logical equality */
    bool expected = aws_array_eq(a.buffer, a.len, b.buffer, b.len);
    assert(result == expected);

    /* Postcondition: buffers remain valid */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Postcondition: length and capacity unchanged */
    assert(a.len == a_old.len);
    assert(a.capacity == a_old.capacity);
    assert(b.len == b_old.len);
    assert(b.capacity == b_old.capacity);

    /* Postcondition: contents unchanged */
    if (a.len > 0) {
        assert(memcmp(a.buffer, a_old_contents, a.len) == 0);
    }
    if (b.len > 0) {
        assert(memcmp(b.buffer, b_old_contents, b.len) == 0);
    }

    /* Clean up */
    if (a.buffer) {
        aws_mem_release(alloc, a.buffer);
    }
    if (b.buffer) {
        aws_mem_release(alloc, b.buffer);
    }

    return 0;
}
