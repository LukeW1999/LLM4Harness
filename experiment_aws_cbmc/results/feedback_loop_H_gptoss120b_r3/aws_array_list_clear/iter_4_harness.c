#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    if (aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_clear(&list);
    assert(result == AWS_OP_SUCCESS);

    /* Length must be zero after clear */
    assert(list.length == 0);

    /* Other fields must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
