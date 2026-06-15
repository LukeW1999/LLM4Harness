#include <aws/common/array_list.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;
    list.data = nondet_voidp();
    list.length = nondet_size_t();
    list.current_size = nondet_size_t();
    list.item_size = nondet_size_t();
    list.alloc = NULL;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t len = aws_array_list_length(&list);
    assert(len == list.length);
    assert(aws_array_list_is_valid(&list));
}
