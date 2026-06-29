#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the item size and list capacity for tractability */
#define MAX_ITEM_SIZE 8
#define MAX_INITIAL_ITEMS 4

void aws_array_list_push_back_harness(void) {
    /* Create a dynamic array list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Non-deterministic item size (at least 1, at most MAX_ITEM_SIZE) */
    size_t item_size;
    __CPROVER_assume(item_size >= 1 && item_size <= MAX_ITEM_SIZE);

    /* Non-deterministic initial allocation */
    size_t initial_items;
    __CPROVER_assume(initial_items <= MAX_INITIAL_ITEMS);

    /* Initialize the list dynamically */
    int init_rc = aws_array_list_init_dynamic(&list, alloc, initial_items, item_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic initial length (within current capacity) */
    size_t initial_length;
    size_t capacity = list.current_size / list.item_size;
    __CPROVER_assume(initial_length <= capacity);
    list.length = initial_length;

    /* Record state before the call */
    size_t old_length = aws_array_list_length(&list);
    size_t old_current_size = list.current_size;

    /* Create a non-deterministic value to push */
    uint8_t val_buf[MAX_ITEM_SIZE];
    /* val_buf is readable for item_size bytes */
    void *val = (void *)val_buf;

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under test */
    int rc = aws_array_list_push_back(&list, val);

    /* Postcondition 1: Return value correctness */
    /* rc must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);

    /* Postcondition 2: List validity is maintained */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 3: Length invariants */
    if (rc == AWS_OP_SUCCESS) {
        /* On success, length must have increased by exactly 1 */
        assert(aws_array_list_length(&list) == old_length + 1);

        /* Current size must be at least enough to hold all elements */
        size_t required_size = aws_array_list_length(&list) * list.item_size;
        assert(list.current_size >= required_size);

        /* Capacity must be >= new length */
        assert(list.current_size / list.item_size >= aws_array_list_length(&list));
    } else {
        /* On failure, length must remain unchanged */
        assert(aws_array_list_length(&list) == old_length);
    }

    /* Postcondition 4: item_size must not change */
    assert(list.item_size == item_size);

    /* Postcondition 5: allocator must not change */
    assert(list.alloc == alloc);

    /* Postcondition 6: current_size must not shrink */
    assert(list.current_size >= old_current_size);

    /* Postcondition 7: If success, data pointer must be non-null */
    if (rc == AWS_OP_SUCCESS) {
        assert(list.data != NULL);
    }

    /* Postcondition 8: Error code consistency */
    if (rc == AWS_OP_ERR) {
        /* On error, the last error should be set */
        int last_err = aws_last_error();
        assert(last_err != 0);
    }

    /* Clean up */
    aws_array_list_clean_up(&list);
}
