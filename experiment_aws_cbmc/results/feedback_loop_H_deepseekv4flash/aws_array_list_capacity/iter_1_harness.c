#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    size_t item_size;
    size_t length;
    size_t current_size;
    void *val;

    /* Bounding assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);

    /* Non-deterministic val pointer */
    val = can_fail_malloc(list.item_size);
    if (val) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* Call function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index is set to val */
        if (index < list.length) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size), (uint8_t *)val, list.item_size);
        } else {
            /* If index >= old length, length becomes index + 1 */
            assert(list.length == index + 1);
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size), (uint8_t *)val, list.item_size);
        }
    } else {
        /* On failure, the list is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
    }

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* Validity invariants */
    assert(aws_array_list_is_valid(&list));
}
