#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(list.alloc == NULL || list.alloc == allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t old_length = list.length;

    size_t length = aws_array_list_length(&list);

    assert(length == old_length);
    assert(aws_array_list_is_valid(&list));
}
