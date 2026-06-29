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
    struct aws_array_list list;

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 &&
                     initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    __CPROVER_assume(initial_item_allocation <= SIZE_MAX / item_size);

    struct aws_allocator *allocator = aws_default_allocator();
    int init_rc = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    if (init_rc != AWS_OP_SUCCESS) {
        return;
    }

    /* Nondeterministically populate the list with some elements */
    size_t num_elements;
    __CPROVER_assume(num_elements <= initial_item_allocation);

    uint8_t *scratch = malloc(item_size);
    if (scratch == NULL) {
        aws_array_list_clean_up(&list);
        return;
    }

    for (size_t i = 0; i < num_elements; i++) {
        int push_rc = aws_array_list_push_back(&list, scratch);
        if (push_rc != AWS_OP_SUCCESS) {
            free(scratch);
            aws_array_list_clean_up(&list);
            return;
        }
    }

    /* Ensure list is valid before calling push_front */
    if (!aws_array_list_is_valid(&list)) {
        free(scratch);
        aws_array_list_clean_up(&list);
        return;
    }

    /* Record state before the call */
    size_t orig_len = aws_array_list_length(&list);
    size_t orig_item_size = list.item_size;
    struct aws_allocator *orig_alloc = list.alloc;

    /* Allocate val with nondeterministic content */
    uint8_t *val = malloc(item_size);
    if (val == NULL) {
        free(scratch);
        aws_array_list_clean_up(&list);
        return;
    }

    /* Save a copy of val for postcondition checking */
    uint8_t *val_copy = malloc(item_size);
    if (val_copy == NULL) {
        free(val);
        free(scratch);
        aws_array_list_clean_up(&list);
        return;
    }
    memcpy(val_copy, val, item_size);

    /* Save a copy of the original list data */
    uint8_t *orig_data = NULL;
    if (orig_len > 0 && list.data != NULL) {
        orig_data = malloc(orig_len * item_size);
        if (orig_data == NULL) {
            free(val_copy);
            free(val);
            free(scratch);
            aws_array_list_clean_up(&list);
            return;
        }
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
}
