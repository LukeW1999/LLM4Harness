#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 for meaningful swap */
    __CPROVER_assume(list.item_size > 0);

    /* The list must have at least one element for valid indices */
    __CPROVER_assume(list.length > 0);

    /* Non-deterministic indices */
    size_t a;
    size_t b;

    /* Preconditions: both indices must be within bounds */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Ensure data pointer is non-null and accessible */
    __CPROVER_assume(list.data != NULL);

    /* Ensure the data region is large enough */
    __CPROVER_assume(list.current_size >= list.length * list.item_size);

    /* Save old state before the call */
    struct aws_array_list old_list = list;

    /* Save the data contents before the swap for verification */
    size_t item_size = list.item_size;

    /* Allocate buffers to save old item contents */
    uint8_t *old_item_a = malloc(item_size);
    uint8_t *old_item_b = malloc(item_size);
    __CPROVER_assume(old_item_a != NULL);
    __CPROVER_assume(old_item_b != NULL);

    /* Copy old items */
    uint8_t *data_ptr = (uint8_t *)list.data;
    memcpy(old_item_a, data_ptr + a * item_size, item_size);
    memcpy(old_item_b, data_ptr + b * item_size, item_size);

    /* 3. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 4. Assert postconditions */

    /* Unchanged fields: alloc, current_size, length, item_size, data pointer */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* 5. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
