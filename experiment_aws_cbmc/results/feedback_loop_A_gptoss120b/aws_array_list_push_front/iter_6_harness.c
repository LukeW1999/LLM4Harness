/* Harness for aws_array_list_push_front */
#include <assert.h>
#include <stdlib.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;

    /* Assume the list is bounded and valid */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.current_size % list.item_size == 0);
    __CPROVER_assume(list.length <= list.alloc);

    /* Save the original state */
    struct aws_array_list old = list;

    /* Allocate a nondeterministic value to push */
    size_t item_sz = list.item_size;
    uint8_t *val = (uint8_t *)malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        /* Verify that the first item matches the pushed value */
        for (size_t i = 0; i < item_sz; ++i) {
            assert(((uint8_t *)list.data)[i] == val[i]);
        }
        assert(list.alloc >= old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
