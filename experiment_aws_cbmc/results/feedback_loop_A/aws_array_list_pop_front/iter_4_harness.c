#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_pop_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t initial_capacity;
    size_t item_size;
    __CPROVER_assume(initial_capacity > 0 && initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_init(&list, initial_capacity, item_size) == AWS_OP_SUCCESS);
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Populate the list with some elements */
    void *item;
    size_t num_items;
    __CPROVER_assume(num_items <= initial_capacity);
    for (size_t i = 0; i < num_items; i++) {
        item = malloc(item_size);
        __CPROVER_assume(item != NULL);
        __CPROVER_assume(aws_array_list_push_back(&list, item) == AWS_OP_SUCCESS);
    }

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 4. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.current_size >= list.length * list.item_size);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.capacity == old.capacity);
    assert(list.item_size == old.item_size);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* 8. Clean up allocated memory */
    for (size_t i = 0; i < list.length; i++) {
        free(AWS_ARRAY_LIST_AT_PTR(&list, i));
    }
    aws_array_list_clean_up(&list);
}
