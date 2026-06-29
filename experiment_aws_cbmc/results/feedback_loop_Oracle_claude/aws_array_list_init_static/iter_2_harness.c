#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 16
#define MAX_ITEM_SIZE 32

void aws_array_list_init_static_harness(void) {
    /* Stack-allocate the list */
    struct aws_array_list list;

    /* Nondeterministic inputs */
    size_t initial_item_allocation;
    size_t item_size;
    size_t len;

    /* Ground-truth preconditions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &len));

    /* Allocate raw_array */
    void *raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    /* Postcondition 1: The list is valid after initialization */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 2: alloc is NULL for static lists */
    assert(list.alloc == NULL);

    /* Postcondition 3: data points to the provided raw_array */
    assert(list.data == raw_array);

    /* Postcondition 4: item_size is set correctly */
    assert(list.item_size == item_size);

    /* Postcondition 5: length is 0 (no elements yet) */
    assert(list.length == 0);

    /* Postcondition 6: current_size equals item_count * item_size */
    assert(list.current_size == len);

    /* Postcondition 7: capacity matches initial_item_allocation */
    assert(aws_array_list_capacity(&list) == initial_item_allocation);

    /* Postcondition 8: length is 0 via the accessor */
    assert(aws_array_list_length(&list) == 0);
}
