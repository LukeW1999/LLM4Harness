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

    void *front = aws_array_list_front(&list);

    if (list.length == 0) {
        __CPROVER_assert(front == NULL, "front of an empty list is NULL");
    } else {
        __CPROVER_assert(front == list.data, "front of a non-empty list is list.data");
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
