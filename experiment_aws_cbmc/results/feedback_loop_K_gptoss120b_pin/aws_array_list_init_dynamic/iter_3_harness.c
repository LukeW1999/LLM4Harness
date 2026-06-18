#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= 10);
    __CPROVER_assume(item_size <= 10);

    bool overflow = false;
    size_t expected_allocation = 0;
    if (item_size != 0 && initial_item_allocation > ((size_t)-1) / item_size) {
        overflow = true;
    } else {
        expected_allocation = initial_item_allocation * item_size;
    }

    int rc = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (rc == AWS_OP_SUCCESS) {
        assert(!overflow);
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.current_size == expected_allocation);
        assert(list.length == 0);
        if (expected_allocation > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
    } else {
        assert(rc == AWS_OP_ERR);
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.data == NULL);
    }

    if (list.data != NULL) {
        aws_mem_release(list.alloc, list.data);
    }
}
