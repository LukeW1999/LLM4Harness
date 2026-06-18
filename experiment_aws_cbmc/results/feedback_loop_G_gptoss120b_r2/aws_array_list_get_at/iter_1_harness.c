#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the whole underlying buffer for later comparison */
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Save the whole struct for unchanged‑field checks */
    struct aws_array_list old = list;

    /* 2. Choose nondeterministic indices within bounds */
    size_t len = aws_array_list_length(&list);
    __CPROVER_assume(len > 0);                     /* swap requires a non‑empty list */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    /* Save the elements at the chosen indices */
    uint8_t *elem_a = malloc(list.item_size);
    uint8_t *elem_b = malloc(list.item_size);
    __CPROVER_assume(elem_a != NULL && elem_b != NULL);
    memcpy(elem_a,
           (uint8_t *)list.data + a * list.item_size,
           list.item_size);
    memcpy(elem_b,
           (uint8_t *)list.data + b * list.item_size,
           list.item_size);

    /* 3. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 4. Postconditions – elements at a and b are swapped */
    assert(memcmp((uint8_t *)list.data + a * list.item_size,
                  elem_b,
                  list.item_size) == 0);
    assert(memcmp((uint8_t *)list.data + b * list.item_size,
                  elem_a,
                  list.item_size) == 0);

    /* All other elements remain unchanged */
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            assert(memcmp((uint8_t *)list.data + i * list.item_size,
                          (uint8_t *)old_data + i * list.item_size,
                          list.item_size) == 0);
        }
    }

    /* 5. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up temporary allocations */
    free(old_data);
    free(elem_a);
    free(elem_b);
}
