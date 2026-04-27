#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;
    size_t index;

    // Initialize list with arbitrary values
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOC, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Ensure index is within bounds for the length of the list
    __CPROVER_assume(index < list.length);

    // Save old state of the list
    struct aws_array_list old_list = list;

    int result = aws_array_list_erase(&list, index);

    // Check frame conditions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length - 1);
        assert(list.item_size == old_list.item_size);
        // Data should have changed due to removal and shifting
        for (size_t i = 0; i < list.length; i++) {
            if (i < index) {
                assert(((char *)list.data)[i * list.item_size] == ((char *)old_list.data)[i * list.item_size]);
            } else {
                assert(((char *)list.data)[i * list.item_size] == ((char *)old_list.data)[(i + 1) * list.item_size]);
            }
        }
    } else {
        // Check frame conditions for failure path
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Check validity invariant
    assert(aws_array_list_is_valid(&list));
}
