#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 2
#define MAX_ITEM_SIZE 8

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    /* bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    /* allocate the data buffer non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);
    /* assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state for later comparisons */
    struct aws_array_list old = list;

    /* val: a non-deterministic buffer large enough to hold the maximum item */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = (const void *)val_buf;

    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    int rval = aws_array_list_set_at(&list, val, index);

    if (rval == AWS_OP_SUCCESS) {
        /* list must remain valid */
        assert(aws_array_list_is_valid(&list));
        /* length must be at least index+1 */
        assert(list.length >= index + 1);
        /* the element at index must equal val */
        assert_bytes_match((const uint8_t *)val,
                           (const uint8_t *)list.data + index * list.item_size,
                           list.item_size);
        /* if index was within the old length, length should not change */
        if (index < old.length) {
            assert(list.length == old.length);
        }
    } else {
        /* on failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));
        /* length remains unchanged (the only modification to length is guarded) */
        assert(list.length == old.length);
        /* allocator and item_size are never modified */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size or data may have changed if ensure_capacity partially
           succeeded before a later overflow, so we avoid asserting them */
    }

    /* invariants that hold regardless of success or failure */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
}
