#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Assume non‑empty list so indices can be chosen */
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.current_size == list.length * list.item_size);

    /* 3. Choose valid indices a and b */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Save the elements at positions a and b */
    uint8_t *old_elem_a = malloc(list.item_size);
    uint8_t *old_elem_b = malloc(list.item_size);
    __CPROVER_assume(old_elem_a != NULL);
    __CPROVER_assume(old_elem_b != NULL);
    int get_a_res = aws_array_list_get_at(&list, old_elem_a, a);
    int get_b_res = aws_array_list_get_at(&list, old_elem_b, b);
    __CPROVER_assume(get_a_res == AWS_OP_SUCCESS);
    __CPROVER_assume(get_b_res == AWS_OP_SUCCESS);

    /* 6. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 7. Post‑condition: fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 8. Post‑condition: elements at a and b are swapped */
    uint8_t *new_elem_a = malloc(list.item_size);
    uint8_t *new_elem_b = malloc(list.item_size);
    __CPROVER_assume(new_elem_a != NULL);
    __CPROVER_assume(new_elem_b != NULL);
    int get_a2_res = aws_array_list_get_at(&list, new_elem_a, a);
    int get_b2_res = aws_array_list_get_at(&list, new_elem_b, b);
    __CPROVER_assume(get_a2_res == AWS_OP_SUCCESS);
    __CPROVER_assume(get_b2_res == AWS_OP_SUCCESS);
    assert_bytes_match(new_elem_a, old_elem_b, list.item_size);
    assert_bytes_match(new_elem_b, old_elem_a, list.item_size);

    /* 9. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
