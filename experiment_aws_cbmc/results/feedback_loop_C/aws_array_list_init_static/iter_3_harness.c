#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();
    size_t calculated_size;

    // Allocate a static array on the stack
    char static_array[item_count * item_size];
    void *raw_array = static_array;

    // Assumptions for the input parameters
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &calculated_size));

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    // Assertions for the success path
    assert(list.alloc == NULL);
    assert(list.current_size == calculated_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
