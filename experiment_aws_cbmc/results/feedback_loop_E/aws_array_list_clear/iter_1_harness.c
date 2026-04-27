#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    /* perform operation under verification */
    aws_array_list_clear(&list);

    /* assertions */
    if (old.data) {
        assert_byte_from_buffer_matches(old.data, &old_byte);
    }

    /* changed fields */
    assert(list.length == 0);

    /* unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
}
