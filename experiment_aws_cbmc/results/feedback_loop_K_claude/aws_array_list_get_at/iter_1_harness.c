#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* Allocate and set up the aws_array_list with nondet values */
    struct aws_array_list list;

    /* item_size must be > 0 */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    /* Bound item_size to keep verification tractable */
    __CPROVER_assume(item_size <= 8);

    /* length and current_size */
    size_t length;
    size_t current_size;

    /* Avoid overflow: length * item_size must not overflow */
    size_t required_size;
    __CPROVER_assume(!__builtin_mul_overflow(length, item_size, &required_size));

    /* current_size must be >= required_size */
    __CPROVER_assume(current_size >= required_size);

    /* Bound length to keep verification tractable */
    __CPROVER_assume(length <= 2);

    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;
    list.alloc = aws_default_allocator();

    /* Set up data pointer according to validity invariant */
    if (current_size == 0) {
        list.data = NULL;
    } else {
        /* Bound current_size to keep verification tractable */
        __CPROVER_assume(current_size <= 64);
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    }

    /* Verify precondition: list is valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondet index */
    size_t index;

    /* Allocate val: must be writable for item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save state before call */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* Postcondition: list is still valid */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: frame conditions - list fields unchanged */
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);

    /* Postcondition: val is still non-null */
    assert(val != NULL);

    /* Postcondition: return value based on index vs length */
    if (index < old_length) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
    } else {
        /* Should fail with INVALID_INDEX */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    }
}
