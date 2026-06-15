#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();
    void *raw_array;

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    raw_array = malloc(item_count * item_size);
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.current_size == item_count * item_size);
    assert(list.data == raw_array);
    assert(aws_array_list_is_valid(&list));
}
