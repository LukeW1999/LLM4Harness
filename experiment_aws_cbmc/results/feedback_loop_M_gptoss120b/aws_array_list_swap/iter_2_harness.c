#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
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
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length > 0);

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save the elements at the two indices */
    uint8_t *old_elem_a = (uint8_t *)malloc(list.item_size);
    uint8_t *old_elem_b = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(old_elem_a != NULL);
    __CPROVER_assume(old_elem_b != NULL);

    aws_array_list_get_at(&list, old_elem_a, a);
    aws_array_list_get_at(&list, old_elem_b, b);

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Verify that the two elements have been swapped */
    uint8_t *new_elem_a = (uint8_t *)malloc(list.item_size);
    uint8_t *new_elem_b = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(new_elem_a != NULL);
    __CPROVER_assume(new_elem_b != NULL);

    aws_array_list_get_at(&list, new_elem_a, a);
    aws_array_list_get_at(&list, new_elem_b, b);

    for (size_t i = 0; i < list.item_size; ++i) {
        assert(new_elem_a[i] == old_elem_b[i]);
        assert(new_elem_b[i] == old_elem_a[i]);
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
