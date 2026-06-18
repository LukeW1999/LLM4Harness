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

    /* 2. Choose indices a and b within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save copies of the two elements that may be swapped */
    size_t item_sz = list.item_size;
    uint8_t *old_a = NULL;
    uint8_t *old_b = NULL;
    if (item_sz > 0) {
        old_a = malloc(item_sz);
        old_b = malloc(item_sz);
        __CPROVER_assume(old_a != NULL);
        __CPROVER_assume(old_b != NULL);
        memcpy(old_a,
               (uint8_t *)list.data + a * item_sz,
               item_sz);
        memcpy(old_b,
               (uint8_t *)list.data + b * item_sz,
               item_sz);
    }

    /* 4. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Post‑condition checks */

    /* Unchanged fields of the list structure */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Swapped element contents */
    if (a == b || item_sz == 0) {
        /* No change when indices are equal or item size is zero */
        if (item_sz > 0) {
            assert(memcmp((uint8_t *)list.data + a * item_sz,
                          old_a,
                          item_sz) == 0);
            assert(memcmp((uint8_t *)list.data + b * item_sz,
                          old_b,
                          item_sz) == 0);
        }
    } else {
        /* Elements at a and b must be exchanged */
        assert(memcmp((uint8_t *)list.data + a * item_sz,
                      old_b,
                      item_sz) == 0);
        assert(memcmp((uint8_t *)list.data + b * item_sz,
                      old_a,
                      item_sz) == 0);
    }

    /* 6. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
