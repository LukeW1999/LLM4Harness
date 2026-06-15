#include <aws/common/array_list.h>
#include <aws/common/assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_clear_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clear(&list);

    /* 4. Post‑condition checks */
    /* length must be reset to zero */
    assert(list.length == 0);
    /* other fields must remain unchanged */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 5. Invariant must hold after clear */
    assert(aws_array_list_is_valid(&list));
}
