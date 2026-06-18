#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Retrieve pointers to the two items and save their contents */
    void *ptr_a = NULL;
    void *ptr_b = NULL;
    int get_res_a = aws_array_list_get_at_ptr(&list, &ptr_a, a);
    int get_res_b = aws_array_list_get_at_ptr(&list, &ptr_b, b);
    __CPROVER_assume(get_res_a == AWS_OP_SUCCESS);
    __CPROVER_assume(get_res_b == AWS_OP_SUCCESS);

    struct store_byte_from_buffer old_a;
    struct store_byte_from_buffer old_b;
    save_byte_from_array((uint8_t *)ptr_a, list.item_size, &old_a);
    save_byte_from_array((uint8_t *)ptr_b, list.item_size, &old_b);

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Retrieve pointers after swap */
    void *ptr_a_after = NULL;
    void *ptr_b_after = NULL;
    int get_res_a2 = aws_array_list_get_at_ptr(&list, &ptr_a_after, a);
    int get_res_b2 = aws_array_list_get_at_ptr(&list, &ptr_b_after, b);
    __CPROVER_assume(get_res_a2 == AWS_OP_SUCCESS);
    __CPROVER_assume(get_res_b2 == AWS_OP_SUCCESS);

    /* 7. Assert that the contents have been swapped */
    assert_byte_from_buffer_matches((const uint8_t *)ptr_a_after, &old_b);
    assert_byte_from_buffer_matches((const uint8_t *)ptr_b_after, &old_a);

    /* 8. Assert unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);

    /* 9. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
