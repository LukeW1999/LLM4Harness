#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* Data structure */
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t index;
    size_t old_length;
    void *val;

    /* Ensure the list is bounded and valid */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the old state */
    old_list = list;
    old_length = list.length;

    /* Prepare the val pointer: it must point to readable memory of item_size bytes */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* The malloc'd memory is nondeterministic; assume it is readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at the given index must be a copy of val */
        size_t offset = index * list.item_size;
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + offset, list.item_size));
        assert_bytes_match((uint8_t *)list.data + offset, (uint8_t *)val, list.item_size);

        /* Length may have increased to index+1 if index was beyond old length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* Item size and alloc must remain unchanged */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Current size must be enough to accommodate at least index+1 elements */
        size_t required_size;
        int no_overflow = !aws_mul_size_checked(index + 1, list.item_size, &required_size);
        assert(no_overflow);
        assert(list.current_size >= required_size);

        /* Data pointer must be writable for current_size bytes */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    } else {
        /* On failure, the list must remain valid and unchanged */
        assert(aws_array_list_is_valid(&list));
        /* The contents of the list must not have been modified (except possibly by ensure_capacity) */
        /* We can only check that the list still satisfies its invariants */
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        /* If the data pointer changed, it's only because ensure_capacity attempted allocation; but on failure the list is still valid */
        /* We cannot assert data pointer equality because ensure_capacity might have freed the old data on failure? */
        /* According to aws_array_list_ensure_capacity, on failure the list is left in a valid state (unchanged or with old data). */
        /* For static lists, the data pointer remains the raw array, so it won't change. */
        /* If dynamic, ensure_capacity may free old data on failure? The spec says it raises error without modifying list. */
        /* Let's assume the list is unchanged: */
        assert(list.data == old_list.data);
    }

    /* Validity invariants always hold */
    assert(aws_array_list_is_valid(&list));

    /* No memory leaks? The val memory is not freed; that's the caller's responsibility. */
}
