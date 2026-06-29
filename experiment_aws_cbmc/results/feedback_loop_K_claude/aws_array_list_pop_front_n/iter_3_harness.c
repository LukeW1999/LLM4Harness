#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEM_SIZE 4
#define MAX_INITIAL_ITEMS 4

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;

    /* Set up list fields non-deterministically but validly */
    size_t item_size;
    size_t capacity;
    __CPROVER_assume(item_size >= 1 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(capacity >= 1 && capacity <= MAX_INITIAL_ITEMS);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Use a static backing buffer to avoid dynamic allocation issues */
    size_t buf_size = item_size * capacity;
    void *data = malloc(buf_size);
    __CPROVER_assume(data != NULL);

    /* Initialize list manually */
    list.alloc        = allocator;
    list.item_size    = item_size;
    list.current_size = buf_size;
    list.data         = data;

    /* Non-deterministic length in [0, capacity] */
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Ensure validity */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Record pre-call state */
    size_t old_length       = list.length;
    size_t old_item_size    = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data          = list.data;

    /* Non-deterministic n */
    size_t n;

    /* Call the function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* Postconditions: structural validity */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions */
    assert(list.item_size    == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.alloc        == old_alloc);
    assert(list.data         == old_data);

    /* Postconditions: length */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    /* Length never exceeds capacity */
    assert(list.length * list.item_size <= list.current_size);

    /* Clean up */
    aws_array_list_clean_up(&list);
}
