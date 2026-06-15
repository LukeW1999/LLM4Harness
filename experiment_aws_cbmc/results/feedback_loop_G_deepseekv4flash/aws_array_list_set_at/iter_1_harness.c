#include <aws/common/array_list.h>

void aws_array_list_set_at_harness() {
    /* Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic input: value pointer and index */
    size_t index;
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* Ensure val points to readable memory of correct size */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state of the list and a byte from its data region */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &storage);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions for success path */
    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            /* Length becomes index + 1 */
            assert(list.length == index + 1);
        } else {
            /* Length unchanged */
            assert(list.length == old.length);
        }
        /* The element at index must equal the input value */
        assert_bytes_match((uint8_t *)list.data + list.item_size * index, val, list.item_size);
    } else {
        /* On failure, the list must be unchanged */
        assert_array_list_equivalence(&list, &old, &storage);
    }

    /* Unchanged fields (on success, current_size may change due to reallocation) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    if (result != AWS_OP_SUCCESS) {
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* Validity invariants always hold */
    assert(aws_array_list_is_valid(&list));
}
