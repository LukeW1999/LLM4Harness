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

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    /* Use nondet item_size bounded for tractability */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the list dynamically */
    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Create a val buffer of item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Non-deterministic index - bound it to keep verification tractable */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data must be non-null */
        assert(list.data != NULL);

        /* On success: length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* The value at index must match what we wrote */
        assert(memcmp((uint8_t *)list.data + (list.item_size * index), val, list.item_size) == 0);

    } else {
        /* On failure: list length should be unchanged */
        assert(list.length == old_length);
    }
}
