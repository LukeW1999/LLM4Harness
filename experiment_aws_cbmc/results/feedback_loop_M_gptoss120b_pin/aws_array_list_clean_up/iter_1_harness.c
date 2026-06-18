#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Non‑deterministically decide whether the list has allocated data */
    if (nondet_bool()) {
        ensure_array_list_has_allocated_data_member(&list);
    } else {
        list.data = NULL;
        list.current_size = 0;
    }

    /* 3. Non‑deterministically decide whether the list has an allocator */
    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    /* 4. Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 5. Save old state for immutability checks (not strictly needed here) */
    struct aws_array_list old = list;

    /* 6. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 7. Post‑conditions: all fields must be zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 8. The list must remain valid after clean‑up */
    assert(aws_array_list_is_valid(&list));
}
