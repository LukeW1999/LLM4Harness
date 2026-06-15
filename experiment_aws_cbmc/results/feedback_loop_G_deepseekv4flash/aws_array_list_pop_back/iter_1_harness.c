#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_erase_harness() {
    /* Non-deterministic array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic index */
    size_t index;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function */
    int result = aws_array_list_erase(&list, index);

    if (index < old_length) {
        /* Success: element removed */
        assert(list.length == old_length - 1);
        assert(list.data == old_data);
        assert(list.current_size == old_current_size);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: index out of bounds */
        assert(list.length == old_length);
        assert(list.data == old_data);
        assert(list.current_size == old_current_size);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(aws_array_list_is_valid(&list));
    }
}
