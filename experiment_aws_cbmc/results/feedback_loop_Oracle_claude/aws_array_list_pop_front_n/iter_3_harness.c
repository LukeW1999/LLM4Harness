#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 4

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;

    /* Use ensure_array_list_has_allocated_data_member to set up a valid list */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    list.item_size = item_size;
    list.current_size = initial_item_allocation * item_size;
    list.alloc = aws_default_allocator();

    /* Allocate backing data */
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    /* Set length to a valid value */
    size_t length;
    __CPROVER_assume(length <= initial_item_allocation);
    list.length = length;

    /* Verify preconditions */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* Non-deterministic n */
    size_t n;

    /* Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* Postcondition 1: The list must still be valid after the operation */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 2: item_size must not change */
    assert(list.item_size == old_item_size);

    /* Postcondition 3: allocator must not change */
    assert(list.alloc == old_alloc);

    /* Postcondition 4: data pointer must not change (memmove in place) */
    assert(list.data == old_data);

    /* Postcondition 5: current_size must not change */
    assert(list.current_size == old_current_size);

    /* Postcondition 6: Length invariants based on n vs old_length */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    /* Postcondition 7: Length must not exceed old length */
    assert(list.length <= old_length);
}
