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
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Bound item_size and current_size/length before assuming validity */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    list.current_size = capacity * list.item_size;
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= capacity);

    list.alloc = aws_default_allocator();

    /* Allocate data buffer */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Bound the index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Create a val buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Unchanged fields regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data must be valid and non-null */
        assert(list.data != NULL);

        /* The list must be valid */
        assert(aws_array_list_is_valid(&list));

        /* item_size unchanged */
        assert(list.item_size == old_list.item_size);

        /* If index >= old length, length becomes index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* index < old length: length unchanged */
            assert(list.length == old_list.length);
        }

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* The value was copied into the list at the correct position */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure: the list should still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, key invariants hold */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }
}
