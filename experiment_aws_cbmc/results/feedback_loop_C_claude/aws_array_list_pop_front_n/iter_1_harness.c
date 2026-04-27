// === STEP 1: SUCCESS PATH ===
// aws_array_list_pop_front_n is void, no return value.
// When n == 0: nothing changes
// When 0 < n < list->length:
//   - list->length: CHANGES to (old_length - n)
//   - list->data: content shifted (memmove), but pointer unchanged
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
// When n >= list->length:
//   - list->length: CHANGES to 0 (aws_array_list_clear sets length=0)
//   - list->data: UNCHANGED pointer
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data (pointer): UNCHANGED always (content may change)
//   - length: CHANGED (set to 0 if n >= old_length, else old_length - n if n > 0, else unchanged)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_pop_front_n_harness() {
    /* Allocate and initialize the list with bounded parameters */
    struct aws_array_list list;
    
    /* Bound the list to make verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    
    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    /* Nondeterministic n */
    size_t n;
    n = nondet_size_t();
    
    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    
    /* Call the function under test */
    aws_array_list_pop_front_n(&list, n);
    
    /* === Frame condition assertions === */
    
    /* alloc is always unchanged */
    assert(list.alloc == old_list.alloc);
    
    /* current_size is always unchanged */
    assert(list.current_size == old_list.current_size);
    
    /* item_size is always unchanged */
    assert(list.item_size == old_list.item_size);
    
    /* data pointer is always unchanged */
    assert(list.data == old_list.data);
    
    /* length changes based on n vs old_length */
    if (n == 0) {
        /* n == 0: nothing changes */
        assert(list.length == old_length);
    } else if (n >= old_length) {
        /* n >= length: clear, length becomes 0 */
        assert(list.length == 0);
    } else {
        /* 0 < n < length: length decreases by n */
        assert(list.length == old_length - n);
    }
    
    /* Validity invariant must hold after call */
    assert(aws_array_list_is_valid(&list));
}
