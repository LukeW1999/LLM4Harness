#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;

    /* Bound the list and allocate its data buffer */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure there is at least one element */
    __CPROVER_assume(list.length > 0);

    /* Choose nondeterministic valid indices */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save old state */
    struct aws_array_list old = list;

    /* Save original elements at indices a and b */
    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    int get_res_a = aws_array_list_get_at(&list, old_a, a);
    assert(get_res_a == AWS_OP_SUCCESS);
    int get_res_b = aws_array_list_get_at(&list, old_b, b);
    assert(get_res_b == AWS_OP_SUCCESS);

    /* Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* Verify that the elements were swapped */
    uint8_t new_a[MAX_ITEM_SIZE];
    uint8_t new_b[MAX_ITEM_SIZE];
    int get_res_a2 = aws_array_list_get_at(&list, new_a, a);
    assert(get_res_a2 == AWS_OP_SUCCESS);
    int get_res_b2 = aws_array_list_get_at(&list, new_b, b);
    assert(get_res_b2 == AWS_OP_SUCCESS);

    assert_bytes_match(new_a, old_b, list.item_size);
    assert_bytes_match(new_b, old_a, list.item_size);

    /* Verify unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Verify list remains valid */
    assert(aws_array_list_is_valid(&list));
}
