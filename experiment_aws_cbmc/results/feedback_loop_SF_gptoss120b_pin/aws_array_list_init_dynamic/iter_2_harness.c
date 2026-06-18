#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_dynamic_harness(void) {
    /* Symbolic inputs */
    struct aws_array_list list = {0};
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_uint64_t();
    size_t item_size = nondet_uint64_t();

    /* Precondition assumptions */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(&list != NULL);

    /* Snapshot of input state */
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    /* Call the function under verification */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Return value must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* The list must be properly initialized */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);

        if (initial_item_allocation == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
    } else {
        /* On error, the list must remain unchanged (frame condition) */
        assert(list.alloc == old_alloc);
        assert(list.item_size == old_item_size);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }
}
