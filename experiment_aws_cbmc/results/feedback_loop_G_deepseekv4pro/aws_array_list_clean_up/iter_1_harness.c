#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 2
#define MAX_ITEM_SIZE 16

void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;

    /* Allocate a valid dynamic list with an allocated data buffer */
    ensure_array_list_has_allocated_data_member(&list);

    /* Bound the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    /* Ensure the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — for memory address checking only */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    aws_array_list_clean_up(&list);

    /* 4. Assert postconditions: struct is zeroed out */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 5. Assert validity invariant always holds after cleanup (empty list is valid) */
    assert(aws_array_list_is_valid(&list));

    /* Memory pointed to by old.data should be freed; CBMC's built-in leak check verifies this.
     * No explicit byte-level assertion needed. */
}
