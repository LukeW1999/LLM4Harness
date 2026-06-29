#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound on item_size and length to keep verification tractable */
#define MAX_ITEM_SIZE 8
#define MAX_LENGTH    4

void aws_array_list_swap_harness(void) {
    /* Allocate and initialize the list struct */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Non-deterministic item_size and length within bounds */
    size_t item_size;
    size_t length;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(length >= 1 && length <= MAX_LENGTH);

    /* current_size must be at least length * item_size and not overflow */
    size_t current_size;
    __CPROVER_assume(current_size >= length * item_size);
    __CPROVER_assume(current_size <= SIZE_MAX);
    /* Ensure no overflow in length * item_size */
    __CPROVER_assume(length * item_size / item_size == length);

    /* Allocate backing data */
    void *data = malloc(current_size);
    __CPROVER_assume(data != NULL);

    list->alloc        = aws_default_allocator();
    list->item_size    = item_size;
    list->length       = length;
    list->current_size = current_size;
    list->data         = data;

    /* Verify the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(list));

    /* Non-deterministic indices within bounds */
    size_t a;
    size_t b;
    __CPROVER_assume(a < list->length);
    __CPROVER_assume(b < list->length);

    /* Snapshot invariants before the call */
    size_t old_length       = list->length;
    size_t old_current_size = list->current_size;
    size_t old_item_size    = list->item_size;
    struct aws_allocator *old_alloc = list->alloc;

    /* Snapshot element values before the call for frame condition checks */
    /* We copy the raw bytes at positions a and b */
    void *snap_a = malloc(item_size);
    void *snap_b = malloc(item_size);
    __CPROVER_assume(snap_a != NULL);
    __CPROVER_assume(snap_b != NULL);

    uint8_t *base = (uint8_t *)list->data;
    for (size_t i = 0; i < item_size; i++) {
        ((uint8_t *)snap_a)[i] = base[a * item_size + i];
        ((uint8_t *)snap_b)[i] = base[b * item_size + i];
    }

    /* Call the function under verification */
    aws_array_list_swap(list, a, b);

    /* Postconditions (validity) */
    assert(aws_array_list_is_valid(list));
    assert(list->data != NULL);

    /* Postconditions (length/capacity invariants unchanged) */
    assert(list->length       == old_length);
    assert(list->current_size == old_current_size);
    assert(list->item_size    == old_item_size);
    assert(list->alloc        == old_alloc);

    /* Postconditions (swap correctness) */
    if (a == b) {
        /* No memory modified: elements at a and b are unchanged */
        for (size_t i = 0; i < item_size; i++) {
            assert(base[a * item_size + i] == ((uint8_t *)snap_a)[i]);
            assert(base[b * item_size + i] == ((uint8_t *)snap_b)[i]);
        }
    } else {
        /* Elements at a and b are swapped */
        for (size_t i = 0; i < item_size; i++) {
            assert(base[a * item_size + i] == ((uint8_t *)snap_b)[i]);
            assert(base[b * item_size + i] == ((uint8_t *)snap_a)[i]);
        }
    }

    return 0;
}
