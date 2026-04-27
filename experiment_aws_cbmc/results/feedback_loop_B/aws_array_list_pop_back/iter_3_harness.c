#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t initial_capacity = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    initialize_bounded_array_list(&list, allocator, initial_capacity, item_size);

    /* Ensure the list is valid before the operation */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Add items to the list to ensure we can pop */
    size_t num_items = nondet_size_t();
    __CPROVER_assume(num_items <= initial_capacity);
    for (size_t i = 0; i < num_items; i++) {
        void *item = malloc(item_size);
        __CPROVER_assume(item != NULL);
        aws_array_list_push_back(&list, item);
        free(item);
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_back(&list);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(list.length == old.length - 1);

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* Struct unchanged on failure */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
