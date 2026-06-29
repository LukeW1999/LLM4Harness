#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the buffer size for tractable verification */
#define MAX_BUFFER_SIZE 4

void aws_byte_cursor_eq_harness(void) {
    /* Allocate two aws_byte_cursor structures */
    struct aws_byte_cursor *a = malloc(sizeof(struct aws_byte_cursor));
    struct aws_byte_cursor *b = malloc(sizeof(struct aws_byte_cursor));

    /* Precondition: pointers must be non-null */
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);

    /* Set up cursor a with nondet length bounded for tractability */
    size_t len_a;
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    a->len = len_a;

    if (a->len > 0) {
        a->ptr = malloc(a->len);
        __CPROVER_assume(a->ptr != NULL);
    } else {
        a->ptr = NULL;
    }

    /* Set up cursor b with nondet length bounded for tractability */
    size_t len_b;
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
    b->len = len_b;

    if (b->len > 0) {
        b->ptr = malloc(b->len);
        __CPROVER_assume(b->ptr != NULL);
    } else {
        b->ptr = NULL;
    }

    /* Precondition: cursors must be valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(a));
    __CPROVER_assume(aws_byte_cursor_is_valid(b));

    /* Save state before the call */
    size_t old_len_a = a->len;
    size_t old_len_b = b->len;
    uint8_t *old_ptr_a = a->ptr;
    uint8_t *old_ptr_b = b->ptr;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(a, b);

    /* Postconditions (frame): cursors are not modified */
    assert(a->len == old_len_a);
    assert(b->len == old_len_b);
    assert(a->ptr == old_ptr_a);
    assert(b->ptr == old_ptr_b);

    /* Postconditions (validity): cursors remain valid */
    assert(aws_byte_cursor_is_valid(a));
    assert(aws_byte_cursor_is_valid(b));

    /* Postcondition: if lengths differ, result must be false */
    if (a->len != b->len) {
        assert(result == false);
    }

    /* Postcondition: if both lengths are 0, result must be true */
    if (a->len == 0 && b->len == 0) {
        assert(result == true);
    }

    /* Postcondition: result is a valid bool */
    assert(result == true || result == false);

    return 0;
}
