#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* Bound and initialize a valid array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Choose indices within the bounds of the list (precondition) */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length && b < list.length);

    /* Save old metadata and a copy of the entire data buffer */
    struct aws_array_list old = list;
    size_t total_size = list.current_size * list.item_size;
    uint8_t *old_data = (uint8_t *)malloc(total_size);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, list.data, total_size);

    /* Call the function under test */
    aws_array_list_swap(&list, a, b);

    /*
     * Postcondition 1: The list remains valid.
     */
    assert(aws_array_list_is_valid(&list));

    /*
     * Postcondition 2: All metadata fields are unchanged.
     */
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /*
     * Postcondition 3: The data contents of the two indices are swapped,
     * and all other indices remain unchanged.
     */
    for (size_t i = 0; i < list.length; i++) {
        if (i == a && i == b) {
            /* a == b : no swap, bytes should be identical to original at that index */
            assert_bytes_match((uint8_t *)list.data + i * list.item_size,
                               old_data + i * list.item_size,
                               list.item_size);
        } else if (i == a) {
            /* i == a && a != b : new bytes at a come from old position b */
            assert_bytes_match((uint8_t *)list.data + a * list.item_size,
                               old_data + b * list.item_size,
                               list.item_size);
        } else if (i == b) {
            /* i == b && a != b : new bytes at b come from old position a */
            assert_bytes_match((uint8_t *)list.data + b * list.item_size,
                               old_data + a * list.item_size,
                               list.item_size);
        } else {
            /* Any other index: bytes unchanged */
            assert_bytes_match((uint8_t *)list.data + i * list.item_size,
                               old_data + i * list.item_size,
                               list.item_size);
        }
    }
}
