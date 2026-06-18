#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;

    /* Bound the array list state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state */
    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    void *val;
    void *old_val = val; /* capture the initial (non-deterministic) value of val */

    int result = aws_array_list_get_at_ptr(&list, &val, index);

    if (result == AWS_OP_SUCCESS) {
        /* On success, val must point to the element at the given index */
        assert(val == (void *)((uint8_t *)old.data + (old.item_size * index)));

        /* The list itself must be unchanged (const parameter) */
        assert(list.data        == old.data);
        assert(list.length      == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size   == old.item_size);
        assert(list.alloc       == old.alloc);
    } else {
        /* On failure, val must be unmodified */
        assert(val == old_val);

        /* The list remains unchanged */
        assert(list.data        == old.data);
        assert(list.length      == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size   == old.item_size);
        assert(list.alloc       == old.alloc);
    }

    /* The list must remain valid after the call (it is const, so invariant holds) */
    assert(aws_array_list_is_valid(&list));
}
