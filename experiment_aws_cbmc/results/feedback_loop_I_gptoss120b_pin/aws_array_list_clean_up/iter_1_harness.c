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
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));

    /* nondeterministically decide whether the list has an allocator */
    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    /* if there is an allocator, ensure the data member is allocated */
    if (list.alloc) {
        ensure_array_list_has_allocated_data_member(&list);
    } else {
        /* when no allocator, the data pointer must be NULL for validity */
        list.data = NULL;
    }

    /* assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Post‑conditions: all fields must be zero after clean_up */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 5. The list must remain a valid (empty) list */
    assert(aws_array_list_is_valid(&list));
}
