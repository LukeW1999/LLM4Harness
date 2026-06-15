#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_n_harness() {
    /* nondeterministic size of n */
    size_t n;

    /* initialize list with bounded size */
    struct aws_array_list list;
    struct aws_array_list old_list;

    /* Bound the list to avoid unrolled loops that exceed state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data pointer is allocated and valid */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid (contains valid length, item_size, etc.) */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state before calling the function */
    old_list = list;

    /* Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* Postconditions */
    /* 1. The list must remain valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* 2. Fields that must not change */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* 3. Length is updated correctly */
    if (n >= old_list.length) {
        /* If n >= old length, list is cleared → length becomes 0 */
        assert(list.length == 0);
    } else if (n > 0) {
        /* Elements 0..n-1 are removed, length decreases by n */
        assert(list.length == old_list.length - n);
    } else {
        /* n == 0: no change */
        assert(list.length == old_list.length);
    }
}
