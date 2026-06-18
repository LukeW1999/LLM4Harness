#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    (void)memset(val, 0, list.item_size);

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(memcmp((uint8_t *)list.data + (index * list.item_size), val, list.item_size) == 0);
        size_t expected_len = (old.length > index) ? old.length : (index + 1);
        assert(list.length == expected_len);
        assert(list.current_size >= old.current_size);
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(aws_array_list_is_valid(&list));

    free(val);
}
