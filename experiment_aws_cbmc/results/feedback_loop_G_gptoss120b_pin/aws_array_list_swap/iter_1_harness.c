#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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

    /* 2. Non-deterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Save copies of the elements at a and b */
    uint8_t *old_elem_a = NULL;
    uint8_t *old_elem_b = NULL;
    if (list.item_size > 0) {
        old_elem_a = malloc(list.item_size);
        old_elem_b = malloc(list.item_size);
        __CPROVER_assume(old_elem_a != NULL);
        __CPROVER_assume(old_elem_b != NULL);
        memcpy(old_elem_a,
               (uint8_t *)list.data + a * list.item_size,
               list.item_size);
        memcpy(old_elem_b,
               (uint8_t *)list.data + b * list.item_size,
               list.item_size);
    }

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Postconditions */

    /* Unchanged fields of the list structure */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (a == b) {
        /* No swap performed, data unchanged */
        if (list.item_size > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + a * list.item_size,
                old_elem_a,
                list.item_size);
        }
    } else {
        /* Elements at a and b are swapped */
        if (list.item_size > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + a * list.item_size,
                old_elem_b,
                list.item_size);
            assert_bytes_match(
                (uint8_t *)list.data + b * list.item_size,
                old_elem_a,
                list.item_size);
        }
        /* All other elements unchanged */
        for (size_t i = 0; i < list.length; ++i) {
            if (i != a && i != b) {
                assert_bytes_match(
                    (uint8_t *)list.data + i * list.item_size,
                    (uint8_t *)old.data + i * old.item_size,
                    list.item_size);
            }
        }
    }

    /* 7. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_elem_a);
    free(old_elem_b);
}
