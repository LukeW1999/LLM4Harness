#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list *list = malloc(sizeof(*list));
    size_t item_count;
    size_t item_size;
    size_t current_size;
    void *raw_array;

    /* Bounds for symbolic values to manage state space */
    __CPROVER_assume(item_count > 0 && item_count <= 1000);
    __CPROVER_assume(item_size > 0 && item_size <= 100);
    __CPROVER_assume(list != NULL);
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Allocate the raw array with exactly the required size */
    raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under verification */
    aws_array_list_init_static(list, raw_array, item_count, item_size);

    /* Assert postconditions */
    __CPROVER_assert(list->alloc == NULL, "alloc should be NULL");
    __CPROVER_assert(list->current_size == current_size, "current_size should be set to product");
    __CPROVER_assert(list->item_size == item_size, "item_size should be set");
    __CPROVER_assert(list->length == 0, "length should be 0");
    __CPROVER_assert(list->data == raw_array, "data should point to raw_array");
    __CPROVER_assert(aws_array_list_is_valid(list), "list should be valid");
}
