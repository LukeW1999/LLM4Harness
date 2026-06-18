#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save original metadata */
    struct aws_array_list old = list;

    /* Save the elements at indices a and b */
    uint8_t *old_a = malloc(list.item_size);
    uint8_t *old_b = malloc(list.item_size);
    __CPROVER_assume(old_a);
    __CPROVER_assume(old_b);
    memcpy(old_a, (uint8_t *)list.data + a * list.item_size, list.item_size);
    memcpy(old_b, (uint8_t *)list.data + b * list.item_size, list.item_size);

    aws_array_list_swap(&list, a, b);

    /* Check that list structure is unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Check that elements were swapped */
    assert(memcmp((uint8_t *)list.data + a * list.item_size, old_b, list.item_size) == 0);
    assert(memcmp((uint8_t *)list.data + b * list.item_size, old_a, list.item_size) == 0);

    /* Ensure list is still valid */
    assert(aws_array_list_is_valid(&list));

    free(old_a);
    free(old_b);
}
