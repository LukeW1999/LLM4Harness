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

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save the elements at a and b before the swap */
    uint8_t *old_val_a = malloc(list.item_size);
    uint8_t *old_val_b = malloc(list.item_size);
    assert(old_val_a != NULL);
    assert(old_val_b != NULL);
    int get_res_a = aws_array_list_get_at(&list, old_val_a, a);
    int get_res_b = aws_array_list_get_at(&list, old_val_b, b);
    assert(get_res_a == AWS_OP_SUCCESS);
    assert(get_res_b == AWS_OP_SUCCESS);

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Verify that the elements at a and b have been swapped */
    uint8_t *new_val_a = malloc(list.item_size);
    uint8_t *new_val_b = malloc(list.item_size);
    assert(new_val_a != NULL);
    assert(new_val_b != NULL);
    int get_res_a2 = aws_array_list_get_at(&list, new_val_a, a);
    int get_res_b2 = aws_array_list_get_at(&list, new_val_b, b);
    assert(get_res_a2 == AWS_OP_SUCCESS);
    assert(get_res_b2 == AWS_OP_SUCCESS);
    assert(memcmp(new_val_a, old_val_b, list.item_size) == 0);
    assert(memcmp(new_val_b, old_val_a, list.item_size) == 0);

    /* 7. Verify that all other elements are unchanged */
    uint8_t *tmp_new = malloc(list.item_size);
    uint8_t *tmp_old = malloc(list.item_size);
    assert(tmp_new != NULL);
    assert(tmp_old != NULL);
    for (size_t i = 0; i < old.length; ++i) {
        if (i == a || i == b) {
            continue;
        }
        int r1 = aws_array_list_get_at(&list, tmp_new, i);
        int r2 = aws_array_list_get_at(&old,  tmp_old, i);
        assert(r1 == AWS_OP_SUCCESS);
        assert(r2 == AWS_OP_SUCCESS);
        assert(memcmp(tmp_new, tmp_old, list.item_size) == 0);
    }

    /* 8. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 9. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(old_val_a);
    free(old_val_b);
    free(new_val_a);
    free(new_val_b);
    free(tmp_new);
    free(tmp_old);
}
