#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    const struct aws_array_list old = list;
    const size_t length = aws_array_list_length(&list);
    const size_t item_size = list.item_size;
    const size_t data_size = list.current_size;

    /* Copy the entire data buffer for later comparison */
    uint8_t *old_data = malloc(data_size);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, list.data, data_size);

    /* Pick indices a and b nondeterministically */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Precondition: both indices must be within the bounds of the array */
    __CPROVER_assume(a < length);
    __CPROVER_assume(b < length);

    /* Call the function under verification */
    aws_array_list_swap(&list, a, b);

    /* 2. Postcondition: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 3. Fields that must remain unchanged */
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* 4. Swap behavior: elements at a and b are exchanged, rest unchanged */
    uint8_t *base = (uint8_t *)list.data;
    uint8_t *old_base = old_data;

    if (a == b) {
        /* No change at all */
        /* Check whole buffer unchanged */
        assert_bytes_match(base, old_base, data_size);
    } else {
        /* Verify swapped elements */
        const uint8_t *elem_a_new = base + (a * item_size);
        const uint8_t *elem_b_new = base + (b * item_size);
        const uint8_t *elem_a_old = old_base + (a * item_size);
        const uint8_t *elem_b_old = old_base + (b * item_size);

        assert_bytes_match(elem_a_new, elem_b_old, item_size);
        assert_bytes_match(elem_b_new, elem_a_old, item_size);

        /* Verify all other elements remain unchanged */
        for (size_t i = 0; i < length; ++i) {
            if (i != a && i != b) {
                const uint8_t *elem_new = base + (i * item_size);
                const uint8_t *elem_old = old_base + (i * item_size);
                assert_bytes_match(elem_new, elem_old, item_size);
            }
        }
    }

    free(old_data);
}
