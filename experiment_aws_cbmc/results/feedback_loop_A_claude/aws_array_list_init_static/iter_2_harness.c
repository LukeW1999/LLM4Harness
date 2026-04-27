#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 16
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 64
#endif

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;

    size_t item_count;
    size_t item_size;

    /* Preconditions from AWS_FATAL_PRECONDITION */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Bound the sizes to keep the state space manageable */
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Ensure item_count * item_size does not overflow */
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    size_t current_size = item_count * item_size;

    /* Allocate raw_array with sufficient size */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 2. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 3. Assert postconditions */

    /* Changed fields - explicitly set by the function */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* 4. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
