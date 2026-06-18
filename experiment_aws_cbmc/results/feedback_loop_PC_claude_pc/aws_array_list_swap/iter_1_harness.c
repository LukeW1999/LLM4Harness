#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic indices */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 3. Preconditions: both indices must be within bounds */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* Save the bytes at positions a and b before the swap */
    /* We need to capture the actual element bytes for postcondition checking */
    size_t item_size = list.item_size;

    /* Allocate buffers to save element contents */
    uint8_t *old_a_data = NULL;
    uint8_t *old_b_data = NULL;

    if (item_size > 0 && list.data != NULL) {
        old_a_data = malloc(item_size);
        old_b_data = malloc(item_size);
        if (old_a_data != NULL && old_b_data != NULL) {
            uint8_t *base = (uint8_t *)list.data;
            __CPROVER_assume(AWS_MEM_IS_READABLE(base + a * item_size, item_size));
            __CPROVER_assume(AWS_MEM_IS_READABLE(base + b * item_size, item_size));
            for (size_t i = 0; i < item_size; i++) {
                old_a_data[i] = base[a * item_size + i];
                old_b_data[i] = base[b * item_size + i];
            }
        }
    }

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert postconditions */

    /* The function returns void, so no return value to check */

    /* FRAME: structural fields must not change */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* SWAP postcondition: if a == b, nothing changes */
    if (a == b) {
        /* Data is unchanged */
        if (item_size > 0 && list.data != NULL && old_a_data != NULL) {
            uint8_t *base = (uint8_t *)list.data;
            for (size_t i = 0; i < item_size; i++) {
                assert(base[a * item_size + i] == old_a_data[i]);
            }
        }
    } else {
        /* After swap: element at position a should have old b's data,
           and element at position b should have old a's data */
        if (item_size > 0 && list.data != NULL && old_a_data != NULL && old_b_data != NULL) {
            uint8_t *base = (uint8_t *)list.data;
            for (size_t i = 0; i < item_size; i++) {
                assert(base[a * item_size + i] == old_b_data[i]);
                assert(base[b * item_size + i] == old_a_data[i]);
            }
        }
    }

    /* INVARIANTS: list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
