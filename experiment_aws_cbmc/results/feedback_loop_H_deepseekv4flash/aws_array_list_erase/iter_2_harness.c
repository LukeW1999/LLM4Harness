#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void harness() {
    struct aws_array_list list;
    size_t index;

    /* Ensure the list is valid and bounded */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Save old state */
    struct aws_array_list old = list;
    size_t old_length = aws_array_list_length(&old);

    /* Call the function */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        size_t new_length = aws_array_list_length(&list);
        assert(new_length == old_length - 1);
        assert(index < old_length);
        if (index > 0) {
            size_t bytes_before = index * list.item_size;
            assert_bytes_match((const uint8_t *)list.data, (const uint8_t *)old.data, bytes_before);
        }
        size_t trailing = (new_length - index) * list.item_size;
        assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                           (const uint8_t *)old.data + (index + 1) * list.item_size,
                           trailing);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        assert(index >= old_length);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert_bytes_match((const uint8_t *)list.data, (const uint8_t *)old.data, old.current_size);
    }

    assert(aws_array_list_is_valid(&list));
}
