#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    size_t index_a;
    size_t index_b;

    /* Setup: allocate and initialize the list */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Use static initialization to avoid dynamic allocation issues */
    void *raw_array = malloc(initial_item_allocation * item_size);
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    /* Set length to something nonzero so we can have valid indices */
    size_t length;
    __CPROVER_assume(length > 0 && length <= initial_item_allocation);
    list.length = length;

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(index_a < aws_array_list_length(&list));
    __CPROVER_assume(index_b < aws_array_list_length(&list));

    /* Save state before the call */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    aws_array_list_swap(&list, index_a, index_b);

    /* Postconditions */

    /* 1. The list remains valid after the swap */
    assert(aws_array_list_is_valid(&list));

    /* 2. Length invariants: length must not change */
    assert(list.length == old_length);

    /* 3. Capacity invariants: current_size must not change */
    assert(list.current_size == old_current_size);

    /* 4. Item size must not change */
    assert(list.item_size == old_item_size);

    /* 5. Frame conditions: data pointer must not change */
    assert(list.data == old_data);

    /* 6. Allocator must not change */
    assert(list.alloc == old_alloc);

    /* 7. The list is still bounded */
    assert(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
}
