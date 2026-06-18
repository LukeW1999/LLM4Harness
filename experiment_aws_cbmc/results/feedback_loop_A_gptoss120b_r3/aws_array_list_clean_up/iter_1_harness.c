#include <aws/common/byte_buf.h>          /* pulls in allocator definitions */
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>

/* The function under test */
void aws_array_list_clean_up(struct aws_array_list *AWS_RESTRICT list);

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));

    /* Allocate the internal data buffer if the list is expected to own one */
    ensure_array_list_has_allocated_data_member(&list);

    /* The allocator may be NULL (static list) or a valid allocator (dynamic list) */
    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    /* Constrain the list to satisfy its own validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for immutability checks (not strictly needed for this function,
       but kept for completeness) */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Post‑conditions ----------------------------------------------------- */
    /* All fields must be zeroed after clean‑up */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 5. Invariants ---------------------------------------------------------- */
    /* The list must remain valid (a zeroed list is considered valid) */
    assert(aws_array_list_is_valid(&list));
}
