#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_array_list old = list;

    size_t allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size);

    int result = aws_array_list_init_dynamic(&list, alloc,
                                             initial_item_allocation,
                                             item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(!overflow);
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == allocation_size);
        if (allocation_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
    } else {
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    assert(aws_array_list_is_valid(&list));
}
