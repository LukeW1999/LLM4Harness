#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t capacity = aws_array_list_capacity(&list);
    
    if (list.item_size > 0) {
        assert(capacity == list.current_size / list.item_size);
    } else {
        assert(capacity == 0);
    }
    assert(aws_array_list_is_valid(&list));
}
