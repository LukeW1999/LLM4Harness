#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 10

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Preconditions: from implementation */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Ensure multiplication doesn't overflow for bounded values */
    __CPROVER_assume(initial_item_allocation == 0 || item_size == 0 ||
                     initial_item_allocation <= SIZE_MAX / item_size);

    /* Save old state (struct will be zeroed) */
    struct aws_array_list old = list;

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Success postconditions */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        if (initial_item_allocation > 0 && item_size > 0) {
            assert(list.current_size == initial_item_allocation * item_size);
            assert(list.data != NULL);
            assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        } else {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        }
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Error: list must be fully zeroed */
        assert(list.alloc == NULL);
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.item_size == 0);
        assert(list.length == 0);
        /* No validity because alloc is NULL, but we still check fields */
        /* (aws_array_list_is_valid would return false) */
    }
}
