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

void aws_array_list_set_at_harness() {
    /* 1. Declare and initialize the array list with bounded parameters */
    struct aws_array_list list;

    /* Use nondet values for item_size and current_size, bounded */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_count;
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize the list manually to ensure validity */
    list.alloc = aws_default_allocator();
    list.item_size = item_size;
    list.current_size = initial_item_count * item_size;
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= initial_item_count);

    if (list.current_size == 0) {
        list.data = NULL;
    } else {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create val: a readable buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Pick a non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    /* item_size and alloc must not change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data was written at index */
        assert(list.data != NULL);

        /* current_size must be at least (index+1)*item_size */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* Length: if index >= old length, new length = index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* The value at index must match val */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* Length must not have changed on failure */
        assert(list.length == old_length);
    }
}
