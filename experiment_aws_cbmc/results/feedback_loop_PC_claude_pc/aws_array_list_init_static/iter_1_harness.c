#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;

    /* item_count and item_size must be > 0 (FATAL_PRECONDITION) and must not overflow */
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Ensure item_count * item_size does not overflow */
    size_t current_size_check;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size_check));

    /* Bound the sizes to keep the state space manageable */
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Allocate a raw array of the required size */
    void *raw_array = malloc(current_size_check);
    __CPROVER_assume(raw_array != NULL);

    /* 2. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 3. Assert postconditions */

    /* The function initializes a static list:
     * - alloc must be NULL (static mode)
     * - current_size must equal item_count * item_size
     * - item_size must equal the provided item_size
     * - length must be 0 (empty list)
     * - data must point to raw_array
     */

    /* Changed fields */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size_check);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Capacity check: capacity = current_size / item_size = item_count */
    assert(aws_array_list_capacity(&list) == item_count);

    /* Length check */
    assert(aws_array_list_length(&list) == 0);

    /* The raw_array pointer itself is unchanged */
    assert(list.data == raw_array);
}
