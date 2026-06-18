#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare the array list */
    struct aws_array_list list;

    /* 2. Assume the list is bounded */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 3. Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 4. Preserve the old state (not strictly needed for this test) */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 6. Post‑conditions: all fields must be cleared */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 7. The list must remain valid after clean‑up */
    assert(aws_array_list_is_valid(&list));
}
