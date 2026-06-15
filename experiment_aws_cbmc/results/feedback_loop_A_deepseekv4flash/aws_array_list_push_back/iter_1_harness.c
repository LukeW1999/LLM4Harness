#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Assume list is bounded and valid */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Assume val is readable for item_size bytes */
    __CPROVER_assume(val != NULL && AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);

    /* Call function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Element at index should match val */
        assert_bytes_match((uint8_t *)list.data + index * list.item_size, (uint8_t *)val, list.item_size);
        /* Length should be max(old.length, index+1) */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        /* Other elements unchanged (only for indices that existed before) */
        for (size_t i = 0; i < old.length; i++) {
            if (i != index) {
                assert_bytes_match((uint8_t *)list.data + i * list.item_size,
                                   (uint8_t *)old.data + i * list.item_size,
                                   list.item_size);
            }
        }
        /* Capacity must be sufficient for new length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure, list should be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
    }

    /* Invariants */
    assert(aws_array_list_is_valid(&list));
}
