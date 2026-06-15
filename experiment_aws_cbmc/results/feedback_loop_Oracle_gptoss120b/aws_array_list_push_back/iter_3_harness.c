#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(allocator->mem_acquire != NULL);
    __CPROVER_assume(allocator->mem_release != NULL);

    struct aws_array_list list;
    memset(&list, 0, sizeof(list));

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 1024);
    size_t elem_size = sizeof(int);
    __CPROVER_assume(elem_size == 0 || capacity <= SIZE_MAX / elem_size);

    if (aws_array_list_init(&list, allocator, capacity, elem_size) != AWS_OP_SUCCESS) {
        return;
    }

    int elem = nondet_int();
    aws_array_list_push_back(&list, &elem);

    aws_array_list_clean_up(&list);
}
