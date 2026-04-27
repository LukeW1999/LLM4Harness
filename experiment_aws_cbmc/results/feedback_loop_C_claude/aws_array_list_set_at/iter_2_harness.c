#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* Setup: create a bounded array list */
    struct aws_array_list list;
    
    /* Use nondet values with explicit bounds */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;
    
    size_t initial_item_count;
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    
    /* Set up current_size */
    __CPROVER_assume(list.current_size == initial_item_count * item_size || 
                     list.current_size == 0);
    
    /* Ensure the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Setup: create a valid val pointer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Choose a nondeterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Frame conditions: item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On success, data must not be NULL */
        assert(list.data != NULL);

        /* On success, length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was >= old_length, length should now be index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }

        /* The data at the index should match val (memcpy was done) */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure: length must be unchanged */
        assert(list.length == old_length);
    }
}
