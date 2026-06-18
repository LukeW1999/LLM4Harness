#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    /* 1. Declare list and set a valid allocator */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    /* 2. Bound the list and allocate its data member */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 3. Ensure the list has at least one element (swap indices must be valid) */
    __CPROVER_assume(list.length > 0);
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* Save copies of the two elements that may be swapped */
    uint8_t *old_a = malloc(list.item_size);
    uint8_t *old_b = malloc(list.item_size);
    __CPROVER_assume(old_a != NULL);
    __CPROVER_assume(old_b != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        old_a[i] = ((uint8_t *)list.data)[a * list.item_size + i];
        old_b[i] = ((uint8_t *)list.data)[b * list.item_size + i];
    }

    /* 5. Call function under test and assume it succeeds */
    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == 0); /* AWS_OP_SUCCESS */

    /* 6. Post‑condition: elements at a and b are swapped (or unchanged if a==b) */
    void *ptr_a = NULL;
    void *ptr_b = NULL;
    aws_array_list_get_at_ptr(&list, &ptr_a, a);
    aws_array_list_get_at_ptr(&list, &ptr_b, b);
    if (a == b) {
        assert_bytes_match((const uint8_t *)ptr_a, old_a, list.item_size);
        assert_bytes_match((const uint8_t *)ptr_b, old_b, list.item_size);
    } else {
        assert_bytes_match((const uint8_t *)ptr_a, old_b, list.item_size);
        assert_bytes_match((const uint8_t *)ptr_b, old_a, list.item_size);
    }

    /* 7. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(old_a);
    free(old_b);
}
