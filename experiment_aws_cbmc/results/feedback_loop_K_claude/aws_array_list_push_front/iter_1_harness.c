#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the list size for tractable verification */
#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 8
#endif

void aws_array_list_push_front_harness(void) {
    /* ---- Setup ---- */
    struct aws_array_list list;

    /* Nondeterministic item_size, bounded for tractability */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Nondeterministic initial allocation, bounded */
    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 &&
                     initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Prevent overflow in initial allocation */
    __CPROVER_assume(initial_item_allocation * item_size <= SIZE_MAX);
    __CPROVER_assume(initial_item_allocation * item_size / item_size == initial_item_allocation);

    /* Initialize a dynamic list */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_rc = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondeterministically populate the list with some elements */
    size_t num_elements;
    __CPROVER_assume(num_elements <= initial_item_allocation);

    /* Allocate a scratch buffer for pushing elements */
    uint8_t *scratch = malloc(item_size);
    __CPROVER_assume(scratch != NULL);

    for (size_t i = 0; i < num_elements; i++) {
        /* Push nondeterministic data */
        int push_rc = aws_array_list_push_back(&list, scratch);
        __CPROVER_assume(push_rc == AWS_OP_SUCCESS);
    }

    /* Verify list is valid before calling push_front */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Record state before the call */
    size_t orig_len = aws_array_list_length(&list);
    size_t orig_item_size = list.item_size;
    struct aws_allocator *orig_alloc = list.alloc;

    /* Allocate val with nondeterministic content of item_size bytes */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save a copy of val for postcondition checking */
    uint8_t *val_copy = malloc(item_size);
    __CPROVER_assume(val_copy != NULL);
    memcpy(val_copy, val, item_size);

    /* Save a copy of the original list data for postcondition checking */
    uint8_t *orig_data = NULL;
    if (orig_len > 0 && list.data != NULL) {
        orig_data = malloc(orig_len * item_size);
        __CPROVER_assume(orig_data != NULL);
        memcpy(orig_data, list.data, orig_len * item_size);
    }

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

        /* The first element must equal val */
        assert(list.data != NULL);
        assert(memcmp(list.data, val_copy, item_size) == 0);

        /* The remaining elements must equal the original elements */
        if (orig_len > 0 && orig_data != NULL) {
            assert(memcmp((uint8_t *)list.data + item_size, orig_data, orig_len * item_size) == 0);
        }
    } else {
        /* On failure, length must be unchanged */
        assert(list.length == orig_len);
    }

    /* ---- Cleanup ---- */
    aws_array_list_clean_up(&list);
    free(scratch);
    free(val);
    free(val_copy);
    if (orig_data != NULL) {
        free(orig_data);
    }

    return 0;
}
