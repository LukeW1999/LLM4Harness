#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the list's allocator before ensuring it has allocated data */
    list.alloc = allocator;

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t capacity = aws_array_list_capacity(&list);

    /* Capacity should be at least the current length */
    assert(capacity >= list.length);

    /* The total allocated size should be sufficient for the capacity */
    if (list.item_size > 0) {
        assert(capacity * list.item_size <= list.current_size);
    }
}
