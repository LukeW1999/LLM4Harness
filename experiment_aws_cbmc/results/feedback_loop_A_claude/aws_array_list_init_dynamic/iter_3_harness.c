#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);
        assert(list.current_size == 0 || list.data != NULL);
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        }
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(result == AWS_OP_ERR);
    }
}
