#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    void *result = aws_array_list_front(&list);

    if (old.length == 0) {
        __CPROVER_assert(result == NULL, "front of empty list is NULL");
    } else {
        __CPROVER_assert(result == old.data, "front points to first element");
    }

    __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged");
    __CPROVER_assert(list.data == old.data, "data unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
