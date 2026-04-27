#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_array_list_push_back_harness() {
    /* Declare and initialize the list */
    struct aws_array_list list;

    /* Bound the list to make verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Create a valid val pointer with item_size readable bytes */
    /* val must not overlap with list.data to avoid memcpy overlap issues */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Ensure val does not overlap with list.data */
    if (list.data != NULL && list.item_size > 0) {
        __CPROVER_assume((uintptr_t)val + list.item_size <= (uintptr_t)list.data ||
                         (uintptr_t)list.data + list.current_size <= (uintptr_t)val);
    }

    /* Call the function under test */
    int result = aws_array_list_push_back(&list, val);

    /* === Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === Frame conditions === */
    /* item_size and alloc are always unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* === Success path === */
        /* length must have increased by 1 */
        assert(list.length == old_list.length + 1);
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null since we have at least one element */
        assert(list.data != NULL);
    } else {
        /* === Failure path === */
        /* length is unchanged */
        assert(list.length == old_list.length);
        /* data is unchanged */
        assert(list.data == old_list.data);
        /* current_size is unchanged */
        assert(list.current_size == old_list.current_size);
    }
}
