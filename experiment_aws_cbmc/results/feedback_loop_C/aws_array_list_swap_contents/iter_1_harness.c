// === STEP 1: SUCCESS PATH ===
// When aws_array_list_swap_contents returns AWS_OP_SUCCESS (or the successful value):
//   - list_a.alloc: CHANGES to list_b.alloc
//   - list_a.current_size: CHANGES to list_b.current_size
//   - list_a.length: CHANGES to list_b.length
//   - list_a.item_size: CHANGES to list_b.item_size
//   - list_a.data: CHANGES to list_b.data
//   - list_b.alloc: CHANGES to list_a.alloc
//   - list_b.current_size: CHANGES to list_a.current_size
//   - list_b.length: CHANGES to list_a.length
//   - list_b.item_size: CHANGES to list_a.item_size
//   - list_b.data: CHANGES to list_a.data

// === STEP 2: FAILURE PATH ===
// When aws_array_list_swap_contents returns AWS_OP_ERR (or fails):
//   - list_a.alloc: UNCHANGED
//   - list_a.current_size: UNCHANGED
//   - list_a.length: UNCHANGED
//   - list_a.item_size: UNCHANGED
//   - list_a.data: UNCHANGED
//   - list_b.alloc: UNCHANGED
//   - list_b.current_size: UNCHANGED
//   - list_b.length: UNCHANGED
//   - list_b.item_size: UNCHANGED
//   - list_b.data: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list_a (struct aws_array_list):
//     - alloc: CHANGED on success, UNCHANGED on failure
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure
//   list_b (struct aws_array_list):
//     - alloc: CHANGED on success, UNCHANGED on failure
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list_a): YES (must hold after call)
//   - aws_array_list_is_valid(&list_b): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_contents_harness() {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    // Initialize the list_a and list_b with arbitrary values
    list_a.alloc = (struct aws_allocator *)nondet_voidp();
    list_a.current_size = nondet_size_t();
    list_a.length = nondet_size_t();
    list_a.item_size = nondet_size_t();
    list_a.data = nondet_voidp();

    list_b.alloc = (struct aws_allocator *)nondet_voidp();
    list_b.current_size = nondet_size_t();
    list_b.length = nondet_size_t();
    list_b.item_size = nondet_size_t();
    list_b.data = nondet_voidp();

    // Save old states
    struct aws_array_list old_list_a = list_a;
    struct aws_array_list old_list_b = list_b;

    // Precondition: list_a and list_b must have the same allocator
    __CPROVER_assume(list_a.alloc == list_b.alloc);

    // Precondition: list_a and list_b must have the same item_size
    __CPROVER_assume(list_a.item_size == list_b.item_size);

    // Precondition: list_a and list_b must not be the same
    __CPROVER_assume(&list_a != &list_b);

    // Call the function under test
    aws_array_list_swap_contents(&list_a, &list_b);

    // Postconditions
    if (aws_array_list_is_valid(&list_a) && aws_array_list_is_valid(&list_b)) {
        // Success path
        assert(list_a.alloc == old_list_b.alloc);
        assert(list_a.current_size == old_list_b.current_size);
        assert(list_a.length == old_list_b.length);
        assert(list_a.item_size == old_list_b.item_size);
        assert(list_a.data == old_list_b.data);

        assert(list_b.alloc == old_list_a.alloc);
        assert(list_b.current_size == old_list_a.current_size);
        assert(list_b.length == old_list_a.length);
        assert(list_b.item_size == old_list_a.item_size);
        assert(list_b.data == old_list_a.data);
    } else {
        // Failure path
        assert(list_a.alloc == old_list_a.alloc);
        assert(list_a.current_size == old_list_a.current_size);
        assert(list_a.length == old_list_a.length);
        assert(list_a.item_size == old_list_a.item_size);
        assert(list_a.data == old_list_a.data);

        assert(list_b.alloc == old_list_b.alloc);
        assert(list_b.current_size == old_list_b.current_size);
        assert(list_b.length == old_list_b.length);
        assert(list_b.item_size == old_list_b.item_size);
        assert(list_b.data == old_list_b.data);
    }

    // Validity invariants
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
