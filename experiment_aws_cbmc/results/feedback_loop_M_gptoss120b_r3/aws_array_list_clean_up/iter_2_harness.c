#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the internal data buffer if needed */
    ensure_array_list_has_allocated_data_member(&list);

    /* Non‑deterministically decide whether the list has an allocator */
    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Post‑conditions: all fields must be zero / NULL */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
}
