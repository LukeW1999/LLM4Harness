#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    size_t item_size;
    size_t length;
    size_t current_size;
    void *val;

    /* Assume valid list state: item_size > 0, length <= capacity, data != NULL */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(current_size >= length * item_size);
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, current_size));

    /* Set list fields */
    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;
    list.alloc = NULL; /* Not used by set_at, but avoid nondet pointer */

    /* Assume val points to readable memory of item_size bytes */
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* Call function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index is set to val */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (list.item_size * index), list.item_size));
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), (uint8_t *)val, list.item_size);

        /* If index >= old.length, length becomes index + 1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure, list is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        if (list.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }
    }

    /* Unchanged fields regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* Validity invariant: ensure list is in a valid state after call */
    assert(list.data != NULL);
    assert(list.item_size > 0);
    assert(list.current_size >= list.length * list.item_size);
}
