#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    /* call function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* assertions */
    assert(aws_array_list_is_valid(&list));

    /* check that the capacity is calculated correctly */
    assert(capacity == list.current_size / list.item_size);

    /* unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
}
