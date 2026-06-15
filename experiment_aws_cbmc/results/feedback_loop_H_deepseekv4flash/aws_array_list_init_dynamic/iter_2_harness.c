#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/proof_allocators.h"

void harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = can_fail_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t initial_item_allocation;
    size_t item_size;

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    int result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == allocator);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        if (initial_item_allocation > 0) {
            assert(list.data != NULL);
            size_t expected_size;
            // The function succeeded, so the multiplication must not have overflowed
            int mul_ret = aws_mul_size_checked(initial_item_allocation, item_size, &expected_size);
            assert(mul_ret == AWS_OP_SUCCESS);
            assert(list.current_size == expected_size);
        } else {
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }
}
