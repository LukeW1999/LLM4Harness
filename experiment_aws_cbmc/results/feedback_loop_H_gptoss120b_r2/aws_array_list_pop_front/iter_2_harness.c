#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;

    /* Bound the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* Use the default allocator */
    list.alloc = aws_default_allocator();

    /* Allocate the internal data buffer */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure the list is non‑empty so pop_front can succeed */
    __CPROVER_assume(list.length > 0);

    /* Save the old state */
    struct aws_array_list old = list;

    /* Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* pop_front must succeed */
    assert(result == AWS_OP_SUCCESS);

    /* Length and current_size must decrease */
    assert(list.length == old.length - 1);
    assert(list.current_size == old.current_size - old.item_size);

    /* Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.item_size == old.item_size);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
