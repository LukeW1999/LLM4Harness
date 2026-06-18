#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_clean_up_harness(void) {
    /* Declare and zero‑initialize the list */
    struct aws_array_list list = {0};

    /* Bound the list (required by other helpers) */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));

    /* No allocator – clean_up will only zero the fields */
    list.alloc = NULL;
    list.data  = NULL;   /* data must be NULL when alloc is NULL */

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state (not used but kept for symmetry with other harnesses) */
    struct aws_array_list old = list;

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* Post‑conditions: all fields must be zero after clean_up */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* The list must remain a valid (empty) list */
    assert(aws_array_list_is_valid(&list));
}
