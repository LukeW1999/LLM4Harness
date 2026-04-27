#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a non-deterministic val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Non-deterministic index */
    size_t index;

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* item_size must not change */
        assert(list.item_size == old_item_size);

        /* allocator must not change */
        assert(list.alloc == old_alloc);

        /* data pointer must be non-null after successful set */
        assert(list.data != NULL);

        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old length, length stays the same */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be large enough to hold index+1 elements */
        assert(list.current_size >= list.length * list.item_size);

        /* current_size must not shrink */
        assert(list.current_size >= old_current_size);

    } else {
        /* On failure: item_size and alloc must not change */
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
