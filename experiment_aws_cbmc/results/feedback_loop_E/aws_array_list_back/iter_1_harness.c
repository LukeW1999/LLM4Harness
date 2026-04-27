#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_back_harness() {
    /* data structure */
    struct aws_array_list list;
    uint8_t val[list.item_size];

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* save old state */
    struct aws_array_list old_list = list;

    /* perform operation under verification */
    int result = aws_array_list_back(&list, val);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);

        size_t last_item_offset = list.item_size * (list.length - 1);
        uint8_t expected_val[list.item_size];
        memcpy(expected_val, (void *)((uint8_t *)list.data + last_item_offset), list.item_size);
        assert_bytes_match(val, expected_val, list.item_size);
    } else {
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
    }

    assert(aws_array_list_is_valid(&list));
}
