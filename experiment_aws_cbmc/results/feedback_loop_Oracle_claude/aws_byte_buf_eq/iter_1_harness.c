#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_eq_harness(void) {
    /* Allocate two aws_byte_buf structures */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* Use nondet sizes bounded for CBMC tractability */
    size_t len_a;
    size_t len_b;
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Initialize buffers with valid data */
    /* Buffer a */
    if (len_a == 0) {
        a.buffer = NULL;
        a.len = 0;
        a.capacity = 0;
        a.allocator = NULL;
    } else {
        a.buffer = malloc(len_a);
        __CPROVER_assume(a.buffer != NULL);
        a.len = len_a;
        a.capacity = len_a;
        a.allocator = aws_default_allocator();
    }

    /* Buffer b */
    if (len_b == 0) {
        b.buffer = NULL;
        b.len = 0;
        b.capacity = 0;
        b.allocator = NULL;
    } else {
        b.buffer = malloc(len_b);
        __CPROVER_assume(b.buffer != NULL);
        b.len = len_b;
        b.capacity = len_b;
        b.allocator = aws_default_allocator();
    }

    /* Preconditions: both buffers must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save state before the call for frame condition checks */
    size_t old_len_a = a.len;
    size_t old_capacity_a = a.capacity;
    uint8_t *old_buffer_a = a.buffer;

    size_t old_len_b = b.len;
    size_t old_capacity_b = b.capacity;
    uint8_t *old_buffer_b = b.buffer;

    /* Call the function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Postcondition 1: Return value correctness
     * If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* Postcondition 1b: If both are empty (len == 0), result must be true */
    if (a.len == 0 && b.len == 0) {
        assert(result == true);
    }

    /* Postcondition 2: Output buffer length/capacity invariants
     * The function must not modify the length or capacity of either buffer */
    assert(a.len == old_len_a);
    assert(a.capacity == old_capacity_a);
    assert(b.len == old_len_b);
    assert(b.capacity == old_capacity_b);

    /* Postcondition 3: Frame conditions
     * The buffer pointers must not be changed */
    assert(a.buffer == old_buffer_a);
    assert(b.buffer == old_buffer_b);

    /* Postcondition 4: Both buffers remain valid after the call */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Postcondition 5: Symmetry - result should be consistent with content comparison
     * If result is true, lengths must be equal */
    if (result == true) {
        assert(a.len == b.len);
    }

    /* Postcondition 6: Reflexivity check - if a and b point to the same buffer with same len,
     * result must be true */
    if (a.buffer == b.buffer && a.len == b.len) {
        assert(result == true);
    }
}
