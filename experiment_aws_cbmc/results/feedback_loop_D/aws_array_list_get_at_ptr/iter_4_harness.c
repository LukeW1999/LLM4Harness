#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;
    void *val;
    size_t index;

    // Initialize list with non-deterministic values
    struct aws_allocator *allocator = aws_default_allocator();
    size_t current_size = nondet_size_t();
    size_t length = nondet_size_t();
    size_t item_size = nondet_size_t();

    // Ensure the list has valid parameters
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(length <= current_size / item_size);

    // Allocate memory for the list data
    uint8_t data[current_size];
    list.alloc = allocator;
    list.current_size = current_size;
    list.length = length;
    list.item_size = item_size;
    list.data = data;

    // Non-deterministic index
    index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    // Save old state of list
    struct aws_array_list old_list = list;

    // Call the function under test
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    // Assertions based on frame conditions and validity invariants
    if (result == AWS_OP_SUCCESS) {
        assert(val == (void *)(data + (item_size * index)));
    } else {
        assert(val == NULL); // val should be unchanged or uninitialized
    }

    // Frame conditions
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
