#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Non-deterministically choose item_size and current_size */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;

    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    /* current_size must be a multiple of item_size */
    __CPROVER_assume(current_size % item_size == 0);
    list.current_size = current_size;

    /* length in items */
    size_t length;
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(length * item_size <= current_size);
    list.length = length;

    /* allocator */
    list.alloc = aws_default_allocator();

    /* data buffer */
    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Verify list is valid before proceeding */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a non-deterministic val buffer of item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Non-deterministic index - keep small to avoid explosion */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* data must be non-null on success */
        assert(list.data != NULL);

        /* current_size must accommodate index+1 items */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* length update */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* The value was actually written */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

    } else {
        /* On failure, list state should be consistent */
        assert(list.length == old_length);
    }
}
