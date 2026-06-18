#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_push_front_harness() {
    /* 1. Declare and initialize the array list with bounded parameters */
    struct aws_array_list list;

    /* Use nondet values with explicit bounds */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize using aws_array_list_init_dynamic with bounded allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Set up list manually to control state */
    list.alloc = allocator;
    list.item_size = item_size;

    /* Bound current_size and length */
    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    /* current_size must be a multiple of item_size or zero */
    __CPROVER_assume(current_size == 0 || (current_size % item_size == 0));
    list.current_size = current_size;

    size_t length;
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    /* length * item_size <= current_size */
    __CPROVER_assume(length * item_size <= current_size);
    list.length = length;

    /* Allocate data buffer */
    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a val buffer of item_size bytes */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        /* item_size must not change */
        assert(list.item_size == old_item_size);
        /* alloc must not change */
        assert(list.alloc == old_alloc);
        /* current_size must be >= old current_size */
        assert(list.current_size >= old_current_size);
    } else {
        /* On failure: list state should be unchanged */
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size == old_current_size);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
