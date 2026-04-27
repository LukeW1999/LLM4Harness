#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;

    // Initialize list with some values
    list.alloc = aws_default_allocator();
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = bounded_malloc(list.current_size * list.item_size);

    // Assume preconditions
    __CPROVER_assume(aws_array_list_is_valid(&list));

    // Call the function under test
    int result = aws_array_list_clean_up(&list);

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == NULL);
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
    } else {
        // Failure path assertions
        assert(list.alloc == aws_default_allocator());
        assert(list.data == NULL || list.current_size == 0);
        assert(list.current_size >= list.length);
        assert(list.item_size > 0);
    }

    // Validity invariant
    assert(result == AWS_OP_SUCCESS || !aws_array_list_is_valid(&list));
}
