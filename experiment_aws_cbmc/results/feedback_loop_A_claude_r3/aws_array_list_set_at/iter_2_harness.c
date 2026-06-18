#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and initialize the array list with bounded parameters */
    struct aws_array_list list;

    /* Use nondet item_size and initial allocation, bounded for tractability */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the list dynamically */
    if (nondet_bool()) {
        /* Dynamic list */
        aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    } else {
        /* Static list with a backing buffer */
        size_t buf_size = initial_item_allocation * item_size;
        void *buf = NULL;
        if (buf_size > 0) {
            buf = malloc(buf_size);
            __CPROVER_assume(buf != NULL);
        }
        aws_array_list_init_static(&list, buf, initial_item_allocation, item_size);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Non-deterministic index */
    size_t index;

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data must be non-null */
        assert(list.data != NULL);

        /* On success: if index >= old length, length becomes index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* The value at index must match what we wrote */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

    } else {
        /* On failure: list length should be unchanged */
        assert(list.length == old_length);
    }
}
