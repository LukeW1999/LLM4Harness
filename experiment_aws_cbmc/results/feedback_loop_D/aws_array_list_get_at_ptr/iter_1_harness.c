// === STEP 1: SUCCESS PATH ===
// When aws_array_list_get_at_ptr returns AWS_OP_SUCCESS (or the successful value):
//   - list.data: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//   - val: CHANGES to point to the element at index

// === STEP 2: FAILURE PATH ===
// When aws_array_list_get_at_ptr returns AWS_OP_ERR (or fails):
//   - list.data: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.alloc: UNCHANGED
//   - val: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - data: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
// val (void **):
//   - val: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;
    void *val;
    size_t index;

    // Initialize list with non-deterministic values
    list.alloc = (struct aws_allocator *)nondet_ptr();
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = (void *)nondet_ptr();

    // Ensure the list has allocated data member
    ensure_array_list_has_allocated_data_member(&list);

    // Non-deterministic index
    index = nondet_size_t();

    // Save old state of list
    struct aws_array_list old_list = list;

    // Call the function under test
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    // Assertions based on frame conditions and validity invariants
    if (result == AWS_OP_SUCCESS) {
        assert(val == (void *)((uint8_t *)old_list.data + (old_list.item_size * index)));
    } else {
        assert(val == NULL || val == (void *)nondet_ptr()); // val should be unchanged or uninitialized
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
