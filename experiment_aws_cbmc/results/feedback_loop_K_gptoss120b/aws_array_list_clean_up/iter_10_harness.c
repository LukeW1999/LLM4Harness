#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(allocator != NULL);
    list.alloc = allocator;

    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size % list.item_size == 0);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);

    list.data = NULL;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
