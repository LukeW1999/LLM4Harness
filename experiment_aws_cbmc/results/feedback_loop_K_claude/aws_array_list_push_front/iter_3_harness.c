#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 8
#endif

void aws_array_list_push_front_harness(void) {
    /* Initialize a dynamic array list */
    struct aws_array_list list;

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 &&
                     initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Prevent overflow in allocation */
    __CPROVER_assume(initial_item_allocation <= SIZE_MAX / item_size);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_rc = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate val with nondeterministic content */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Record state before the call */
    size_t orig_len = list.length;
    size_t orig_item_size = list.item_size;
    struct aws_allocator *orig_alloc = list.alloc;
    size_t orig_current_size = list.current_size;

    /* ---- Call the function under test ---- */
    int result = aws_array_list_push_front(&list, val);

    /* ---- Postcondition checks ---- */

    /* List must remain valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc must not change */
    assert(list.item_size == orig_item_size);
    assert(list.alloc == orig_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* Length must have increased by 1 */
        assert(list.length == orig_len + 1);

        /* Capacity must be sufficient */
        assert(list.current_size >= list.length * list.item_size);

        /* The list data must not be NULL */
        assert(list.data != NULL);
    } else {
        /* On failure, length must be unchanged */
        assert(list.length == orig_len);
    }

    /* ---- Cleanup ---- */
    aws_array_list_clean_up(&list);
    free(val);
}
