Looking at the function `aws_array_list_swap`:

1. **What fields change on success?** The data at indices `a` and `b` are swapped in memory. The list metadata (alloc, current_size, length, item_size, data pointer) remain unchanged.
2. **What fields are unchanged?** alloc, current_size, length, item_size, data pointer - all metadata fields.
3. **What happens on failure?** The function has `AWS_FATAL_PRECONDITION` (aborts if violated), so we must assume `a < list->length` and `b < list->length`. The function returns void.
4. **What validity invariants must hold?** `aws_array_list_is_valid` must hold after the call.

```c
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic indices */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 3. Preconditions: indices must be within bounds (AWS_FATAL_PRECONDITION) */
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* Save bytes at positions a and b before the swap */
    uint8_t *data_ptr = (uint8_t *)list.data;
    
    /* We need to save the content at index a and b to verify the swap */
    /* Use bounded item_size for the save */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert metadata fields are unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Assert validity invariant holds after the call */
    assert(aws_array_list_is_valid(&list));
}
