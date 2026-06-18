#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    /* copy the old data buffer because the function modifies it in place */
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    size_t old_data_len = old.length * old.item_size;
    for (size_t i = 0; i < old_data_len; i++) {
        old_data[i] = ((uint8_t *)old.data)[i];
    }

    int result = aws_array_list_pop_front(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert(list.length == old.length - 1);
        assert_bytes_match((uint8_t *)list.data,
                           old_data + old.item_size,
                           (old.length - 1) * old.item_size);
    } else {
        assert(old.length == 0);
        assert(list.length == 0);
        assert_bytes_match((uint8_t *)list.data, old_data, old_data_len);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_is_valid(&list));
}
