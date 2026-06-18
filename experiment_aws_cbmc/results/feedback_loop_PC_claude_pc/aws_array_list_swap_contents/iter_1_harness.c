#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness() {
    /* 1. Declare and set up list_a */
    struct aws_array_list list_a;
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_a);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));

    /* 2. Declare and set up list_b */
    struct aws_array_list list_b;
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_b);
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 3. Preconditions from the implementation:
     *    - list_a->alloc must be non-null
     *    - list_a->alloc == list_b->alloc
     *    - list_a->item_size == list_b->item_size
     *    - list_a != list_b
     */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    /* list_a and list_b are separate stack variables, so list_a != list_b is guaranteed */

    /* 4. Save old state before the call */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 5. Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 6. Assert postconditions:
     *    The function swaps the contents of list_a and list_b.
     *    After the swap:
     *      - list_a should have the old contents of list_b
     *      - list_b should have the old contents of list_a
     */

    /* list_a now has old list_b's fields */
    assert(list_a.alloc == old_b.alloc);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.data == old_b.data);

    /* list_b now has old list_a's fields */
    assert(list_b.alloc == old_a.alloc);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.data == old_a.data);

    /* 7. Validity invariants must hold after the swap */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* 8. The allocators and item sizes are preserved (same values, just swapped) */
    /* Since alloc_a == alloc_b (precondition), both lists still have the same allocator */
    assert(list_a.alloc == list_b.alloc);
    /* Since item_size_a == item_size_b (precondition), both lists still have the same item_size */
    assert(list_a.item_size == list_b.item_size);
}
