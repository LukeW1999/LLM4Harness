#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_capacity_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.alloc == NULL || list.alloc == allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    size_t capacity = aws_array_list_capacity(&list);

    assert(capacity == list.current_size / list.item_size);
}
