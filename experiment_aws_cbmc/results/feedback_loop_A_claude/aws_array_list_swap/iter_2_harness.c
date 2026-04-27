#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Preconditions for item_size */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 3. Non-deterministic indices */
    size_t a;
    size_t b;

    /* 4. Preconditions: indices must be within bounds (AWS_FATAL_PRECONDITION) */
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 5. Save old metadata */
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    /* 6. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 7. Assert metadata fields are unchanged */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);

    /* 8. Assert validity invariant holds after the call */
    assert(aws_array_list_is_valid(&list));
}
