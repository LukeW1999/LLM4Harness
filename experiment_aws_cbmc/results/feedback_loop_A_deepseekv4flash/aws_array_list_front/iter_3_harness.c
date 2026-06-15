#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic index */
    size_t index;

    /* Non-deterministic val pointer (must be readable for item_size bytes) */
    const void *val;
    __CPROVER_assume(val != NULL && AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within the current length */
        assert(index < list.length);
        /* The element at index must be copied */
        assert(0 == memcmp((uint8_t *)list.data + index * list.item_size, val, list.item_size));
        /* Fields that must remain unchanged */
        assert(list.length == old_length);
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
    } else {
        /* Failure: list must be unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
    }
}
