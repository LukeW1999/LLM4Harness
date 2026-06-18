#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    void *raw_array = nondet_voidp();
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Bound inputs to avoid state explosion */
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(item_count > 0 && item_count <= 10);
    __CPROVER_assume(item_size > 0 && item_size <= 10);

    /* Avoid overflow in multiplication */
    size_t current_size = item_count * item_size;

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(aws_array_list_is_valid(&list));
}
