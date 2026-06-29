#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound on item size and list capacity for tractable verification */
#define MAX_ITEM_SIZE 4
#define MAX_INITIAL_ITEM_ALLOCATION 4

void aws_array_list_set_at_harness(void) {
    /* ---- Set up the array list ---- */
    struct aws_array_list list;

    /* Choose between dynamic and static list non-deterministically */
    bool use_dynamic;
    __CPROVER_assume(use_dynamic == true || use_dynamic == false);

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    if (use_dynamic) {
        size_t initial_item_allocation;
        __CPROVER_assume(initial_item_allocation > 0 &&
                         initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

        int rc = aws_array_list_init_dynamic(
            &list,
            aws_default_allocator(),
            initial_item_allocation,
            item_size);
        /* If init fails, skip */
        __CPROVER_assume(rc == AWS_OP_SUCCESS);
    } else {
        /* Static list */
        size_t item_count;
        __CPROVER_assume(item_count > 0 && item_count <= MAX_INITIAL_ITEM_ALLOCATION);

        void *raw_array = malloc(item_count * item_size);
        __CPROVER_assume(raw_array != NULL);

        aws_array_list_init_static(&list, raw_array, item_count, item_size);
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* ---- Set up val ---- */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* val contains arbitrary readable bytes — no further constraint needed */

    /* ---- Set up index ---- */
    size_t index;
    /* Leave index unconstrained to test all paths */

    /* ---- Capture pre-call state ---- */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* ---- Call the function under test ---- */
    int result = aws_array_list_set_at(&list, val, index);

    /* ---- Postconditions ---- */

    /* Return value must be AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* item_size and alloc must be unchanged */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    /* List must still be valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* data must be non-NULL on success */
        assert(list.data != NULL);

        /* Length postconditions */
        if (index >= old_length) {
            /* length should have been updated to index + 1 */
            assert(list.length == index + 1);
        } else {
            /* length should be unchanged */
            assert(list.length == old_length);
        }

        /* Length must not exceed capacity */
        assert(list.length * list.item_size <= list.current_size);

        /* The value at index should match val */
        void *stored = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(memcmp(stored, val, list.item_size) == 0);
    } else {
        /* On error, length should be unchanged */
        assert(list.length == old_length);
    }

    /* ---- Clean up ---- */
    aws_array_list_clean_up(&list);

    return 0;
}
