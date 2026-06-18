#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare and set up inputs */
    struct aws_array_list list;

    /* item_count and item_size must be > 0 (fatal preconditions) */
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Must not overflow when multiplied */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Bound the sizes to keep state space manageable */
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Allocate a raw array of the required size */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 2. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 3. Assert postconditions */

    /* Changed fields: */
    /* alloc is set to NULL (static mode) */
    assert(list.alloc == NULL);

    /* current_size = item_count * item_size */
    assert(list.current_size == current_size);

    /* item_size is set to the provided item_size */
    assert(list.item_size == item_size);

    /* length is initialized to 0 */
    assert(list.length == 0);

    /* data points to the provided raw_array */
    assert(list.data == raw_array);

    /* 4. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
