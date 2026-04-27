// === STEP 1: SUCCESS PATH ===
// aws_array_list_swap_contents is void, always succeeds if preconditions met.
// After the call:
//   - list_a->alloc: CHANGES to old list_b->alloc (but they must be equal, so same value)
//   - list_a->current_size: CHANGES to old list_b->current_size
//   - list_a->length: CHANGES to old list_b->length
//   - list_a->item_size: CHANGES to old list_b->item_size (but they must be equal, so same value)
//   - list_a->data: CHANGES to old list_b->data
//   - list_b->alloc: CHANGES to old list_a->alloc (same value due to precondition)
//   - list_b->current_size: CHANGES to old list_a->current_size
//   - list_b->length: CHANGES to old list_a->length
//   - list_b->item_size: CHANGES to old list_a->item_size (same value due to precondition)
//   - list_b->data: CHANGES to old list_a->data
//
// === STEP 2: FAILURE PATH ===
// Function is void and uses AWS_FATAL_PRECONDITION (aborts on failure), no error return path.
//
// === STEP 3: FRAME CONDITIONS ===
// list_a (struct aws_array_list):
//   - alloc: CHANGED to list_b->alloc (same value due to precondition alloc_a == alloc_b)
//   - current_size: CHANGED to old list_b->current_size
//   - length: CHANGED to old list_b->length
//   - item_size: CHANGED to old list_b->item_size (same value due to precondition)
//   - data: CHANGED to old list_b->data
// list_b (struct aws_array_list):
//   - alloc: CHANGED to old list_a->alloc (same value)
//   - current_size: CHANGED to old list_a->current_size
//   - length: CHANGED to old list_a->length
//   - item_size: CHANGED to old list_a->item_size (same value)
//   - data: CHANGED to old list_a->data
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list_a): YES (must hold after call)
//   - aws_array_list_is_valid(&list_b): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_contents_harness() {
    /* Allocate two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* Ensure they have allocated data members */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* Preconditions required by the function:
     * 1. list_a->alloc must be non-null
     * 2. list_a->alloc == list_b->alloc
     * 3. list_a->item_size == list_b->item_size
     * 4. list_a != list_b (they are different variables, so this is always true)
     */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);

    /* Ensure both lists are valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* Verify frame conditions: list_a now has list_b's old contents */
    assert(list_a.alloc == old_b.alloc);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.data == old_b.data);

    /* Verify frame conditions: list_b now has list_a's old contents */
    assert(list_b.alloc == old_a.alloc);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.data == old_a.data);

    /* Verify validity invariants after the call */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
