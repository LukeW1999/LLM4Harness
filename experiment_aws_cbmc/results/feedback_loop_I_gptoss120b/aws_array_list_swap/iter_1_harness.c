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

    /* Ensure there is at least one element so that a and b can be < length */
    __CPROVER_assume(list.length > 0);

    /* 2. Choose nondeterministic indices that satisfy preconditions */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save old state before the call */
    struct aws_array_list old = list;

    /* Save copies of the whole data buffer for later comparison */
    uint8_t *old_data = NULL;
    if (list.data != NULL) {
        old_data = malloc(list.length * list.item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data,
               (uint8_t *)list.data,
               list.length * list.item_size);
    }

    /* 4. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Post‑condition checks */

    /* 5a. Fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);

    /* 5b. Data content checks */
    if (list.data != NULL) {
        if (a == b) {
            /* No change when indices are equal */
            assert(memcmp((uint8_t *)list.data + a * list.item_size,
                          old_data + a * list.item_size,
                          list.item_size) == 0);
        } else {
            /* Element at a should now be the old element at b */
            assert(memcmp((uint8_t *)list.data + a * list.item_size,
                          old_data + b * list.item_size,
                          list.item_size) == 0);
            /* Element at b should now be the old element at a */
            assert(memcmp((uint8_t *)list.data + b * list.item_size,
                          old_data + a * list.item_size,
                          list.item_size) == 0);
            /* All other elements must be unchanged */
            for (size_t i = 0; i < list.length; ++i) {
                if (i != a && i != b) {
                    assert(memcmp((uint8_t *)list.data + i * list.item_size,
                                  old_data + i * list.item_size,
                                  list.item_size) == 0);
                }
            }
        }
    }

    /* 6. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up auxiliary allocation */
    free(old_data);
}
