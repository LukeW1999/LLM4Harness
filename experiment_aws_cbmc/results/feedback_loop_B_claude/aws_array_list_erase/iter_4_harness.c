#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 2
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_erase_harness(void) {
    /* 1. Declare and initialize the array list */
    struct aws_array_list list;

    /* Ensure bounded for tractability */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 for meaningful operations */
    __CPROVER_assume(list.item_size > 0);

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, index must have been valid (< length) */
        assert(index < old_length);

        /* Length decreases by 1 */
        assert(list.length == old_length - 1);

        /* item_size never changes */
        assert(list.item_size == old_item_size);

        /* alloc never changes */
        assert(list.alloc == old_alloc);

        /* current_size never changes (erase doesn't reallocate) */
        assert(list.current_size == old_current_size);

        /* Validity invariant holds */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure, index must have been out of bounds */
        assert(index >= old_length);

        /* Nothing should have changed */
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size == old_current_size);

        /* Validity invariant holds */
        assert(aws_array_list_is_valid(&list));
    }
}
