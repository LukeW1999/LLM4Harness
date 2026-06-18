#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

void aws_array_list_capacity_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_size > 0 && item_size < 1024);
    size_t initial_capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(initial_capacity < 1024);

    int init_res = aws_array_list_init(&list, allocator, initial_capacity, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t cap = aws_array_list_capacity(&list);

    /* The capacity must be at least the current length */
    assert(cap >= list.length);
    /* Capacity expressed in number of items should match the underlying buffer size */
    assert(cap * list.item_size == list.current_size);

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
