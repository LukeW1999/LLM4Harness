#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_clear_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Call function under test */
    aws_array_list_clear(&list);

    /* 4. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* After clear, length must be 0 */
    assert(list.length == 0);

    /* item_size must not change */
    assert(list.item_size == old_list.item_size);

    /* allocator must not change */
    assert(list.alloc == old_list.alloc);

    /* current_size must not change */
    assert(list.current_size == old_list.current_size);

    /* data pointer must not change */
    assert(list.data == old_list.data);
}
