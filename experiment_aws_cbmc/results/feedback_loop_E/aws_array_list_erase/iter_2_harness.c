#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_erase_harness() {
    /* data structure */
    struct aws_array_list list;

    /* parameters */
    size_t index;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(index < list.length); // Ensure index is within bounds

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* perform operation under verification */
    int result = aws_array_list_erase(&list, index);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        if (index < old.length - 1) {
            assert_bytes_match((uint8_t *)list.data + index * list.item_size, (uint8_t *)old.data + (index + 1) * list.item_size, (old.length - index - 1) * list.item_size);
        }
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, old.current_size);
    }

    assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    assert(aws_array_list_is_valid(&list));
}
