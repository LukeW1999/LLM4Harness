#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t old_length = old.length;

    /* Create a local copy of the entire allocated buffer to preserve original contents */
    uint8_t *old_data = malloc(old.current_size);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, old.data, old.current_size);

    int result = aws_array_list_pop_front(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(old_length > 0);
        assert(list.length == old_length - 1);
        /* The remaining items should be shifted left by one element */
        assert_bytes_match(list.data, old_data + old.item_size, list.length * list.item_size);
    } else {
        assert(old_length == 0);
        assert(list.length == 0);
        /* Buffer must remain completely untouched when popping from empty list */
        assert_bytes_match(list.data, old_data, old.current_size);
    }

    /* Structural invariants */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_is_valid(&list));

    free(old_data);
}
