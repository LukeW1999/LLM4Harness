#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    /* perform operation under verification */
    int result = aws_array_list_pop_back(&list);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        /* Check that the last item is zeroed */
        size_t last_item_offset = old.item_size * (old.length - 1);
        uint8_t zeroed[old.item_size];
        memset(zeroed, 0, old.item_size);
        assert_bytes_match((uint8_t *)list.data + last_item_offset, zeroed, old.item_size);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    }

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
}
