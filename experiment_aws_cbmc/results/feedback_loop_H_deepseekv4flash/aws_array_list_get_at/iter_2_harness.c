#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_get_at_harness() {
    /* Data structure */
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Ensure the list is bounded and valid */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the old state */
    struct aws_array_list old_list = list;

    /* Prepare the val pointer: it must point to writable memory of item_size bytes */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* The malloc'd memory is nondeterministic; assume it is writable */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, index must be within bounds */
        assert(index < list.length);
        /* The element at index must have been copied to val */
        size_t offset = index * list.item_size;
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + offset, list.item_size));
        assert_bytes_match((uint8_t *)list.data + offset, (uint8_t *)val, list.item_size);
        /* The list must remain unchanged */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    } else {
        /* On failure, index must be out of bounds */
        assert(index >= list.length);
        /* The list must remain unchanged */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        /* val is not modified (but we cannot assert its content because it's nondet) */
    }

    /* Validity invariants always hold */
    assert(aws_array_list_is_valid(&list));
}
