#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEN 4

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* Allocate and initialize aws_byte_cursor */
    struct aws_byte_cursor *a = malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(a != NULL);

    /* Allocate and initialize aws_byte_buf */
    struct aws_byte_buf *b = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(b != NULL);

    /* Set up cursor a with nondet length bounded for tractability */
    size_t a_len;
    __CPROVER_assume(a_len <= MAX_LEN);
    a->len = a_len;

    uint8_t *a_ptr;
    if (a_len > 0) {
        a_ptr = malloc(a_len);
        __CPROVER_assume(a_ptr != NULL);
    } else {
        a_ptr = NULL;
    }
    a->ptr = a_ptr;

    /* Set up buf b with nondet length bounded for tractability */
    size_t b_len;
    __CPROVER_assume(b_len <= MAX_LEN);
    b->len = b_len;

    size_t b_capacity;
    __CPROVER_assume(b_capacity >= b_len);
    b->capacity = b_capacity;

    uint8_t *b_buffer;
    if (b_capacity > 0) {
        b_buffer = malloc(b_capacity);
        __CPROVER_assume(b_buffer != NULL);
    } else {
        b_buffer = NULL;
    }
    b->buffer = b_buffer;
    b->allocator = NULL;

    /* Precondition checks */
    __CPROVER_assume(aws_byte_cursor_is_valid(a));
    __CPROVER_assume(aws_byte_buf_is_valid(b));

    /* Save state before call */
    size_t old_a_len = a->len;
    uint8_t *old_a_ptr = a->ptr;
    size_t old_b_len = b->len;
    size_t old_b_capacity = b->capacity;
    uint8_t *old_b_buffer = b->buffer;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(a, b);

    /* Postcondition: frame conditions - inputs not modified */
    assert(a->len == old_a_len);
    assert(a->ptr == old_a_ptr);
    assert(b->len == old_b_len);
    assert(b->capacity == old_b_capacity);
    assert(b->buffer == old_b_buffer);

    /* Postcondition: if lengths differ, result must be false */
    if (a->len != b->len) {
        assert(result == false);
    }

    /* Postcondition: if result is true, lengths must be equal */
    if (result == true) {
        assert(a->len == b->len);
    }

    /* Postcondition: validity of structures preserved */
    assert(aws_byte_cursor_is_valid(a));
    assert(aws_byte_buf_is_valid(b));

    /* Postcondition: result is a valid bool */
    assert(result == true || result == false);
}
