#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Need at least 1 element to have valid indices */
    __CPROVER_assume(list.length > 0);

    /* 3. Non-deterministic indices within bounds */
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old structural state */
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data_ptr = list.data;

    /* 5. Save copies of the bytes at positions a and b before the swap */
    uint8_t *elem_a_before = malloc(list.item_size);
    uint8_t *elem_b_before = malloc(list.item_size);
    __CPROVER_assume(elem_a_before != NULL);
    __CPROVER_assume(elem_b_before != NULL);

    uint8_t *data_bytes = (uint8_t *)list.data;
    __CPROVER_array_copy(elem_a_before, data_bytes + a * list.item_size);
    __CPROVER_array_copy(elem_b_before, data_bytes + b * list.item_size);

    /* Use memcpy to copy the bytes */
    for (size_t i = 0; i < list.item_size; i++) {
        elem_a_before[i] = data_bytes[a * list.item_size + i];
        elem_b_before[i] = data_bytes[b * list.item_size + i];
    }

    /* 6. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 7. Assert structural postconditions */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data_ptr);

    /* 8. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* 9. Check swap correctness */
    uint8_t *new_data_bytes = (uint8_t *)list.data;

    if (a == b) {
        /* Data unchanged */
        for (size_t i = 0; i < list.item_size; i++) {
            assert(new_data_bytes[a * list.item_size + i] == elem_a_before[i]);
        }
    } else {
        /* Element at position a in new list == element that was at position b */
        for (size_t i = 0; i < list.item_size; i++) {
            assert(new_data_bytes[a * list.item_size + i] == elem_b_before[i]);
        }
        /* Element at position b in new list == element that was at position a */
        for (size_t i = 0; i < list.item_size; i++) {
            assert(new_data_bytes[b * list.item_size + i] == elem_a_before[i]);
        }
    }
}
