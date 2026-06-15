#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;

    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    size_t current_size = item_count * item_size;

    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(list.current_size == current_size);
    assert(aws_array_list_is_valid(&list));
}
