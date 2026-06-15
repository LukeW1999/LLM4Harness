#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= 256);

    size_t init_capacity = nondet_size_t();
    __CPROVER_assume(init_capacity <= 64);

    if (aws_array_list_init(&list, alloc, init_capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    aws_array_list_clean_up(&list);

    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == NULL);
    assert(list.item_size == old.item_size);
}
