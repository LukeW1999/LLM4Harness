#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_array_list old_list = list;

    // Ensure the allocator is valid
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        if (initial_item_allocation > 0 && item_size > 0) {
            assert(list.current_size == initial_item_allocation);
            assert(list.data != NULL);
#ifdef DEBUG_BUILD
            assert(aws_byte_buf_is_bounded((struct aws_byte_buf *)&list.data, list.current_size));
#endif
        } else {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        }
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    // Check validity of the list
    assert(aws_array_list_is_valid(&list));

    // Clean up
    aws_array_list_clean_up(&list);
}
