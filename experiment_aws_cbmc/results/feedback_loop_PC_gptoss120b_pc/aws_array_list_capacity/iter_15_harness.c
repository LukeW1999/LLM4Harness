#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity <= 64);

    if (aws_array_list_init(&list, allocator, initial_capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    size_t push_count = nondet_size_t();
    __CPROVER_assume(push_count <= list.current_capacity);
    for (size_t i = 0; i < push_count; ++i) {
        uint8_t elem[64];
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = nondet_uint8_t();
        }
        int r = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(r == AWS_OP_SUCCESS);
    }

    struct aws_array_list old = list;

    size_t cap = aws_array_list_capacity(&list);

    __CPROVER_assert(cap == list.current_capacity, "capacity matches internal field");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    aws_array_list_clean_up(&list);
}
