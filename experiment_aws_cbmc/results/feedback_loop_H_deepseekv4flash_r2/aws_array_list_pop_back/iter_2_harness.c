#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    /* Step 1: Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Step 2: Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* Step 3: Call function under test */
    int result = aws_array_list_pop_back(&list);

    /* Step 4: Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Success: length decreased by 1 */
        assert(list.length == old.length - 1);
        /* Data pointer and item_size unchanged */
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* The remaining elements are unchanged */
        if (list.length > 0) {
            assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, list.length * list.item_size);
        }
    } else {
        /* Failure: list must be empty */
        assert(old.length == 0);
        /* List unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (list.data != NULL && list.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, list.current_size);
        }
    }
}
