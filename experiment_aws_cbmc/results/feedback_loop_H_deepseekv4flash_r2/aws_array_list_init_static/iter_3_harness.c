#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    void *raw_array = nondet_voidp();
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Assume preconditions from the function's fatal assertions */
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    /* Ensure multiplication does not overflow */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postcondition: fields set as documented */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
