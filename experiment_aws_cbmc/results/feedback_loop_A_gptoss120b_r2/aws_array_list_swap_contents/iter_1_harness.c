#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Preconditions required by the function */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b); /* distinct objects */

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 3. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 4. Assert postconditions */

    /* Allocators and item sizes are unchanged (they were equal) */
    assert(list_a.alloc == old_a.alloc);
    assert(list_b.alloc == old_b.alloc);
    assert(list_a.item_size == old_a.item_size);
    assert(list_b.item_size == old_b.item_size);

    /* Fields are swapped */
    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);
    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);
    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* 5. Validity invariants must still hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
