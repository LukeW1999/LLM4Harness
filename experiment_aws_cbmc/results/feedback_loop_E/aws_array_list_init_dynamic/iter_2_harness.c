#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_dynamic_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    struct aws_allocator *alloc;
    size_t initial_item_allocation;
    size_t item_size;

    /* assumptions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* save old state */
    struct aws_array_list old_list = list;

    /* perform operation under verification */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }

    /* unchanged fields */
    assert(list.alloc == (result == AWS_OP_SUCCESS ? alloc : old_list.alloc));
    assert(list.item_size == (result == AWS_OP_SUCCESS ? item_size : old_list.item_size));
    assert(list.length == 0);
    assert(list.current_size == (result == AWS_OP_SUCCESS ? initial_item_allocation * item_size : old_list.current_size));
    assert(list.data == (result == AWS_OP_SUCCESS ? list.data : old_list.data));

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
}
