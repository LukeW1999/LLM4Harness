#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure data is allocated if current_size > 0 */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume list is valid before call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Assume val points to readable memory of size list->item_size */
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list->item_size));

    /* Save old state for comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    /* Save the old data at the target index if it exists */
    if (index < old.length && old.data != NULL) {
        save_byte_from_array((uint8_t *)old.data + (index * old.item_size), old.item_size, &old_data);
    }

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: data at index should match val */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size), (uint8_t *)val, list.item_size);

        /* Length update: if index >= old length, new length = index + 1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* item_size and alloc should remain unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        /* Failure: list should be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        /* If old data was valid, ensure it's unchanged */
        if (index < old.length && old.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data + (index * list.item_size), &old_data);
        }
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
