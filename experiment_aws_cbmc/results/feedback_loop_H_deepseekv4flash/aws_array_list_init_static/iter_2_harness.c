#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    /* Non-deterministic parameters */
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Assumptions to ensure valid inputs and prevent overflow */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size); /* prevent overflow in multiplication */
    void *raw_array = malloc(item_count * item_size);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions: all fields are set as expected */
    assert(list.alloc == NULL);
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.current_size == item_count * item_size);
    assert(list.data == raw_array);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
