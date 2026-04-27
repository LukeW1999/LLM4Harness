#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_clear_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    void *old_alloc = list.alloc;

    /* 3. Call function under test */
    aws_array_list_clear(&list);

    /* 4. Assert postconditions */

    /* After clear, length must be 0 */
    assert(list.length == 0);

    /* item_size, alloc, data, current_size must not change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);

    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));
}
