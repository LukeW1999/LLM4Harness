#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    /* Provide a valid allocator for the list (required for aws_mem_release) */
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    aws_array_list_clean_up(&list);

    /* 4. Assert postconditions */

    /* All fields must be zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 5. No other fields exist, so nothing else to assert as unchanged */

    /* 6. Validity invariant must still hold (zeroed list is a valid empty list) */
    assert(aws_array_list_is_valid(&list));
}
