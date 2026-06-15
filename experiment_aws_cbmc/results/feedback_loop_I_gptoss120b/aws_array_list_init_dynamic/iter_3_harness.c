#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_init_dynamic_harness(void) {
    /* start with a clean list structure */
    struct aws_array_list list;
    list.alloc = NULL;
    list.item_size = 0;
    list.length = 0;
    list.current_size = 0;
    list.data = NULL;

    /* non‑NULL allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* nondeterministic but bounded inputs */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(initial_item_allocation == 0 ||
                     initial_item_allocation <= SIZE_MAX / item_size);

    /* call the function under test */
    int result = aws_array_list_init_dynamic(
        &list, alloc, initial_item_allocation, item_size);

    /* post‑conditions */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);
    assert(list.length == 0);

    if (result == AWS_OP_SUCCESS) {
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            assert(list.current_size == initial_item_allocation * item_size);
            assert(list.data != NULL);
        }
    } else {
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* invariant */
    assert(aws_array_list_is_valid(&list));
}
