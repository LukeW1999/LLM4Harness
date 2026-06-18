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

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_set_at_harness() {
    /* 1. Declare and set up the array list with bounded parameters */
    struct aws_array_list list;

    /* Bound item_size and current_size before calling ensure */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_count;
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize the list fields directly with bounded values */
    list.alloc = aws_default_allocator();
    list.item_size = item_size;
    list.length = initial_item_count;
    list.current_size = initial_item_count * item_size;

    /* Allocate data if current_size > 0 */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Ensure the list is valid before proceeding */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Bound the index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Create a val buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Fields that must never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* The data pointer must be non-null */
        assert(list.data != NULL);

        /* current_size must be >= (index+1)*item_size after ensure_capacity */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* Length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length should be unchanged */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* The value was copied into the list at the correct position */
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val,
            list.item_size);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }
}
