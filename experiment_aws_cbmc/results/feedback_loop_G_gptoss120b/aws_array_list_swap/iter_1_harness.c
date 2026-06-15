#include <assert.h>
#include <stdlib.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Choose indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save the original items at indices a and b */
    uint8_t *old_item_a = malloc(list.item_size);
    uint8_t *old_item_b = malloc(list.item_size);
    assert(old_item_a != NULL);
    assert(old_item_b != NULL);

    /* Retrieve original items */
    int get_res_a = aws_array_list_get_at(&list, old_item_a, a);
    int get_res_b = aws_array_list_get_at(&list, old_item_b, b);
    assert(get_res_a == AWS_OP_SUCCESS);
    assert(get_res_b == AWS_OP_SUCCESS);

    /* 4. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Retrieve items after swap */
    uint8_t *new_item_a = malloc(list.item_size);
    uint8_t *new_item_b = malloc(list.item_size);
    assert(new_item_a != NULL);
    assert(new_item_b != NULL);

    int get_res_a2 = aws_array_list_get_at(&list, new_item_a, a);
    int get_res_b2 = aws_array_list_get_at(&list, new_item_b, b);
    assert(get_res_a2 == AWS_OP_SUCCESS);
    assert(get_res_b2 == AWS_OP_SUCCESS);

    /* 6. Assert that the items have been swapped */
    assert_bytes_match(new_item_a, old_item_b, list.item_size);
    assert_bytes_match(new_item_b, old_item_a, list.item_size);

    /* 7. Assert unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_item_a);
    free(old_item_b);
    free(new_item_a);
    free(new_item_b);
}
