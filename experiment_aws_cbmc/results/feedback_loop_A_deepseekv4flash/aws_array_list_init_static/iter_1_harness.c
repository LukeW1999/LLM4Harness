#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list *list = malloc(sizeof(*list));
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();
    size_t current_size;
    void *raw_array;

    /* Assumptions to bound state space and satisfy preconditions */
    __CPROVER_assume(list != NULL);
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Allocate raw_array of sufficient size */
    raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under proof */
    aws_array_list_init_static(list, raw_array, item_count, item_size);

    /* Assert postconditions */
    assert(list->alloc == NULL);
    assert(list->current_size == current_size);
    assert(list->item_size == item_size);
    assert(list->length == 0);
    assert(list->data == raw_array);
    assert(aws_array_list_is_valid(list));
}
