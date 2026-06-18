#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* 3. Choose indices respecting preconditions */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(list.length > 0);               /* length must be non‑zero for a,b to be valid */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Postconditions */

    /* Unchanged fields (always) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (a == b) {
        /* No swap performed – data unchanged */
        if (old_data != NULL && list.current_size > 0) {
            assert(memcmp(list.data, old_data, list.current_size) == 0);
        }
    } else {
        /* Elements at a and b are swapped, all other bytes unchanged */
        size_t item_sz = list.item_size;
        size_t offset_a = a * item_sz;
        size_t offset_b = b * item_sz;

        /* Swapped regions */
        for (size_t i = 0; i < item_sz; ++i) {
            assert(((uint8_t *)list.data)[offset_a + i] ==
                   ((uint8_t *)old_data)[offset_b + i]);
            assert(((uint8_t *)list.data)[offset_b + i] ==
                   ((uint8_t *)old_data)[offset_a + i]);
        }

        /* All other bytes unchanged */
        for (size_t i = 0; i < list.current_size; ++i) {
            bool in_a = (i >= offset_a) && (i < offset_a + item_sz);
            bool in_b = (i >= offset_b) && (i < offset_b + item_sz);
            if (!in_a && !in_b) {
                assert(((uint8_t *)list.data)[i] == ((uint8_t *)old_data)[i]);
            }
        }
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data);
}
