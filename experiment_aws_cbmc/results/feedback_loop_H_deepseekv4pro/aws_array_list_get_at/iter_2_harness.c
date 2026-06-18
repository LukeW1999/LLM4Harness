#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    size_t index;

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)val, (uint8_t *)list.data + (index * item_size), item_size);
    } else {
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    if (old.current_size > 0) {
        assert(list.data == old.data);
    }
    assert(aws_array_list_is_valid(&list));

    free(val);
}
