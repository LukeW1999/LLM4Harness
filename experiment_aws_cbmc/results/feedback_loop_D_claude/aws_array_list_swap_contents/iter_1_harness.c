// === STEP 1: SUCCESS PATH ===
// aws_array_list_swap_contents always succeeds (void return, no error path)
// after the call:
//   - list_a->alloc: CHANGES to old list_b->alloc
//   - list_a->current_size: CHANGES to old list_b->current_size
//   - list_a->length: CHANGES to old list_b->length
//   - list_a->item_size: CHANGES to old list_b->item_size
//   - list_a->data: CHANGES to old list_b->data
//   - list_b->alloc: CHANGES to old list_a->alloc
//   - list_b->current_size: CHANGES to old list_a->current_size
//   - list_b->length: CHANGES to old list_a->length
//   - list_b->item_size: CHANGES to old list_a->item_size
//   - list_b->data: CHANGES to old list_a->data
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and uses AWS_FATAL_PRECONDITION (aborts on failure)
//
// === STEP 3: FRAME CONDITIONS ===
// list_a (struct aws_array_list):
//   - alloc: CHANGED (swapped with list_b)
//   - current_size: CHANGED (swapped with list_b)
//   - length: CHANGED (swapped with list_b)
//   - item_size: CHANGED (swapped with list_b)
//   - data: CHANGED (swapped with list_b)
// list_b (struct aws_array_list):
//   - alloc: CHANGED (swapped with list_a)
//   - current_size: CHANGED (swapped with list_a)
//   - length: CHANGED (swapped with list_a)
//   - item_size: CHANGED (swapped with list_a)
//   - data: CHANGED (swapped with list_a)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list_a): YES (must hold after call)
//   - aws_array_list_is_valid(list_b): YES (must hold after call)
//   Preconditions:
//     - list_a->alloc != NULL
//     - list_a->alloc == list_b->alloc
//     - list_a->item_size == list_b->item_size
//     - list_a != list_b

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_contents_harness(void) {
    /* Allocate two distinct array lists */
    struct aws_array_list *list_a = malloc(sizeof(struct aws_array_list));
    struct aws_array_list *list_b = malloc(sizeof(struct aws_array_list));

    /* Ensure they are non-null and distinct */
    __CPROVER_assume(list_a != NULL);
    __CPROVER_assume(list_b != NULL);
    __CPROVER_assume(list_a != list_b);

    /* Set up list_a with bounded nondeterministic values */
    ensure_array_list_has_allocated_data_member(list_a);
    ensure_array_list_has_allocated_data_member(list_b);

    /* Preconditions required by the function */
    /* list_a->alloc must be non-null */
    __CPROVER_assume(list_a->alloc != NULL);
    /* list_a->alloc == list_b->alloc */
    list_b->alloc = list_a->alloc;
    /* list_a->item_size == list_b->item_size */
    list_b->item_size = list_a->item_size;

    /* Ensure both lists are valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(list_a));
    __CPROVER_assume(aws_array_list_is_valid(list_b));

    /* Save old state */
    struct aws_array_list old_a = *list_a;
    struct aws_array_list old_b = *list_b;

    /* Call the function under test */
    aws_array_list_swap_contents(list_a, list_b);

    /* === Verify frame conditions and swap correctness === */

    /* list_a should now contain old list_b's contents */
    assert(list_a->alloc == old_b.alloc);
    assert(list_a->current_size == old_b.current_size);
    assert(list_a->length == old_b.length);
    assert(list_a->item_size == old_b.item_size);
    assert(list_a->data == old_b.data);

    /* list_b should now contain old list_a's contents */
    assert(list_b->alloc == old_a.alloc);
    assert(list_b->current_size == old_a.current_size);
    assert(list_b->length == old_a.length);
    assert(list_b->item_size == old_a.item_size);
    assert(list_b->data == old_a.data);

    /* Validity invariants must hold after the call */
    assert(aws_array_list_is_valid(list_a));
    assert(aws_array_list_is_valid(list_b));
}
