#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound on list size to keep CBMC tractable */
#define MAX_ITEM_SIZE 4
#define MAX_INITIAL_ITEMS 4

void aws_array_list_pop_front_n_harness(void) {
    /* Allocate and initialize the list */
    struct aws_array_list list;

    /* Non-deterministic item_size and initial allocation */
    size_t item_size;
    size_t initial_item_count;

    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_count > 0 && initial_item_count <= MAX_INITIAL_ITEMS);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize a dynamic list */
    int init_result = aws_array_list_init_dynamic(
        &list,
        allocator,
        initial_item_count,
        item_size);

    /* Only proceed if initialization succeeded */
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);

    /* Non-deterministically populate the list with some elements */
    size_t num_elements;
    __CPROVER_assume(num_elements <= initial_item_count);

    /* Push num_elements items onto the list */
    size_t i;
    for (i = 0; i < num_elements; i++) {
        /* Create a non-deterministic element of item_size bytes */
        void *item = malloc(item_size);
        __CPROVER_assume(item != NULL);
        int push_result = aws_array_list_push_back(&list, item);
        free(item);
        /* If push fails, stop adding elements */
        if (push_result != AWS_OP_SUCCESS) {
            break;
        }
    }

    /* Ensure the list is valid before calling the function */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Record pre-call state */
    size_t old_length      = list.length;
    size_t old_item_size   = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* Non-deterministic n */
    size_t n;

    /* Call the function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* Postconditions: structural validity */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: these fields must not change */
    assert(list.item_size    == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.alloc        == old_alloc);
    assert(list.data         == old_data);

    /* Postconditions: length */
    if (n >= old_length) {
        /* All elements removed */
        assert(list.length == 0);
    } else if (n == 0) {
        /* No elements removed */
        assert(list.length == old_length);
    } else {
        /* Exactly n elements removed from front */
        assert(list.length == old_length - n);
    }

    /* Postcondition: length never exceeds capacity */
    assert(list.length * list.item_size <= list.current_size);

    /* Clean up */
    aws_array_list_clean_up(&list);
}
