#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list, 100, 1000));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.current_size >= list.length * list.item_size);

    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;

    void *first_element = malloc(list.item_size);
    __CPROVER_assume(first_element != NULL);

    int rval = aws_array_list_front(&list, first_element);

    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.length == old_length);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(rval == AWS_OP_SUCCESS);

    free(first_element);
}
