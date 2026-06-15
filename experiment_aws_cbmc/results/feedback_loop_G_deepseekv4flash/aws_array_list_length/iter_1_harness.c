#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 10
#endif
#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 10
#endif

void aws_array_list_set_at_harness() {
    /* ------- declare and bound inputs ------- */
    struct aws_array_list list;
    size_t index;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* ------- save old state ------- */
    struct aws_array_list old_list = list;

    /* ------- allocate and prepare val ------- */
    size_t item_size = list.item_size;
    __CPROVER_assume(item_size > 0);               /* already guaranteed by is_valid */
    uint8_t *val = (uint8_t *)can_fail_malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Bound index to avoid infinite loops / massive allocations */
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE); /* safe proxy */

    /* ------- call the function ------- */
    int result = aws_array_list_set_at(&list, val, index);

    /* ------- postconditions ------- */

    /* 1. Return-value-dependent */
    if (result == AWS_OP_SUCCESS) {
        /* data at index *must* equal val */
        size_t offset = index * item_size;
        if (offset + item_size <= list.current_size) {
            assert_bytes_match((uint8_t *)list.data + offset, val, item_size);
        }
        /* length update */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
    } else { /* failure */
        /* On failure, the list remains valid (data might be partially modified,
         * but the function guarantees validity before return). */
    }

    /* 2. Unchanged fields */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    /* 3. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* ------- clean up ------- */
    free(val);
}
