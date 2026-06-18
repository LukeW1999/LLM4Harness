#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    size_t item_count;
    size_t item_size;
    void *raw_array;

    item_count = nondet_size_t();
    item_size  = nondet_size_t();

    __CPROVER_assume(item_count > 0 && item_count <= 4);
    __CPROVER_assume(item_size > 0 && item_size <= 4);

    raw_array = malloc(item_count * item_size);
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.data == raw_array);
    assert(list.item_size == item_size);
    assert(list.length == 0UL);
    assert(list.current_size == item_count * item_size);

    assert(aws_array_list_is_valid(&list));

    free(raw_array);
}
