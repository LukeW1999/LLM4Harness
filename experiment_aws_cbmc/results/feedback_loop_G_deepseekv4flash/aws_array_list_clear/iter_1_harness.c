#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index;

    /* Bound list and ensure allocated data member */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Create a non-deterministic buffer for val of size list->item_size */
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Ensure index is within capacity so set_at will succeed */
    size_t capacity = list.current_size / list.item_size;
    __CPROVER_assume(index < capacity);

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Element at index must now equal val */
        assert_bytes_match((uint8_t *)val, (uint8_t *)list.data + index * item_size, item_size);

        /* Length must be updated if index exceeds old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure, list must be unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }
    }

    /* Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
