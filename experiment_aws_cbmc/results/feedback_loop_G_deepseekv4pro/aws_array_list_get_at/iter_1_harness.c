/* aws_array_list_sort_harness.c */
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * A simple deterministic comparator that compares the first byte of each element.
 * This makes the sortedness property easy to check.
 */
static int compare(const void *a, const void *b) {
    uint8_t va = *(const uint8_t *)a;
    uint8_t vb = *(const uint8_t *)b;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

void aws_array_list_sort_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_sort(&list, compare);

    /* 4. Postconditions */

    /* Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must not change */
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* The list must be sorted according to the comparator */
    size_t len = list.length;
    if (len > 1) {
        /* Bounded loop – CBMC knows len <= bound from is_bounded */
        for (size_t i = 1; i < len; i++) {
            const uint8_t *prev = (const uint8_t *)list.data + (i - 1) * list.item_size;
            const uint8_t *curr = (const uint8_t *)list.data + i * list.item_size;
            assert(compare(prev, curr) <= 0);
        }
    }
}
