#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);

    size_t elem_size = sizeof(int);
    if (aws_array_list_init(&list, allocator, capacity, elem_size) != AWS_OP_SUCCESS) {
        return 0;
    }

    int elem = nondet_int();
    aws_array_list_push_back(&list, &elem);

    aws_array_list_clean_up(&list);
    return 0;
}
