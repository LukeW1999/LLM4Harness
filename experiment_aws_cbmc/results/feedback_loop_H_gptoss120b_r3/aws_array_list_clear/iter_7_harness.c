#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 8

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    if (aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    aws_array_list_clear(&list);

    assert(list.length == 0);
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == 0);

    aws_array_list_clean_up(&list);
}
