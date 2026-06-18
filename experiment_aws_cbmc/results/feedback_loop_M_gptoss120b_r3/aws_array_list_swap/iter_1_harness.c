#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
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

    /* Ensure there is at least one element so that indices can be chosen */
    __CPROVER_assume(list.length > 0);

    /* 2. Save old state */
    struct aws_array_list old = list;
    /* Save a copy of the underlying data buffer for later comparison */
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* 3. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Assert fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert that the elements at indices a and b are swapped and all others unchanged */
    if (list.item_size > 0 && list.length > 0) {
        for (size_t i = 0; i < list.length; ++i) {
            uint8_t *new_elem = (uint8_t *)list.data + i * list.item_size;
            uint8_t *old_elem = (uint8_t *)old_data + i * list.item_size;

            if (i == a) {
                uint8_t *expected = (uint8_t *)old_data + b * list.item_size;
                assert(memcmp(new_elem, expected, list.item_size) == 0);
            } else if (i == b) {
                uint8_t *expected = (uint8_t *)old_data + a * list.item_size;
                assert(memcmp(new_elem, expected, list.item_size) == 0);
            } else {
                assert(memcmp(new_elem, old_elem, list.item_size) == 0);
            }
        }
    }

    /* 7. Assert validity invariant holds after the call */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data);
}
