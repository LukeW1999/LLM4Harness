#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare inputs */
    struct aws_array_list list;

    /* item_count and item_size must be > 0 (fatal preconditions) */
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Multiplication must not overflow (fatal precondition) */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Allocate a raw_array of sufficient size */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 2. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 3. Assert postconditions */

    /* Fields written by the function */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* 4. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
