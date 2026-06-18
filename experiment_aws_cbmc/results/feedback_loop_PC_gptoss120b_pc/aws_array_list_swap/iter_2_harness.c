#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);          /* item_size must be non‑zero */
    __CPROVER_assume(list.length > 0);            /* list must contain at least one element */

    /* 2. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save copies of the elements at positions a and b */
    uint8_t *old_elem_a = malloc(list.item_size);
    uint8_t *old_elem_b = malloc(list.item_size);
    __CPROVER_assume(old_elem_a != NULL);
    __CPROVER_assume(old_elem_b != NULL);
    memcpy(old_elem_a,
           (uint8_t *)old.data + a * old.item_size,
           old.item_size);
    memcpy(old_elem_b,
           (uint8_t *)old.data + b * old.item_size,
           old.item_size);

    /* 4. Call the function under test and assume success */
    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    /* 5. Postconditions */

    /* 5.1. Fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 5.2. Element contents */
    for (size_t i = 0; i < list.length; ++i) {
        uint8_t *cur = (uint8_t *)list.data + i * list.item_size;
        uint8_t *old_cur = (uint8_t *)old.data + i * old.item_size;
        if (i == a && i == b) {
            /* a == b: element unchanged */
            assert_bytes_match(cur, old_cur, list.item_size);
        } else if (i == a) {
            /* a receives former b */
            assert_bytes_match(cur, old_elem_b, list.item_size);
        } else if (i == b) {
            /* b receives former a */
            assert_bytes_match(cur, old_elem_a, list.item_size);
        } else {
            /* all other elements unchanged */
            assert_bytes_match(cur, old_cur, list.item_size);
        }
    }

    /* 5.3. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_elem_a);
    free(old_elem_b);
}
