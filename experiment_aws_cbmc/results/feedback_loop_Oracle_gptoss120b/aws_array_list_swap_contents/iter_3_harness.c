#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_swap_contents_harness(void) {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* structural validity assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_b.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);          /* same allocator */
    __CPROVER_assume(list_a.item_size > 0);
    __CPROVER_assume(list_b.item_size > 0);
    __CPROVER_assume(list_a.item_size == list_b.item_size); /* same item size */
    __CPROVER_assume(&list_a != &list_b);                   /* distinct objects */

    /* capture pre‑state */
    size_t a_len_before   = list_a.length;
    size_t b_len_before   = list_b.length;
    size_t a_cur_before   = list_a.current_size;
    size_t b_cur_before   = list_b.current_size;
    size_t item_sz        = list_a.item_size;
    void *a_data_before   = list_a.data;
    void *b_data_before   = list_b.data;

    /* safety assumptions for copying */
    __CPROVER_assume(a_len_before * item_sz <= a_cur_before);
    __CPROVER_assume(b_len_before * item_sz <= b_cur_before);
    __CPROVER_assume(!(a_data_before == b_data_before && a_len_before > 0 && b_len_before > 0));

    uint8_t *a_snapshot = NULL;
    uint8_t *b_snapshot = NULL;

    if (a_data_before != NULL && a_len_before > 0) {
        a_snapshot = malloc(a_len_before * item_sz);
        __CPROVER_assume(a_snapshot != NULL);
        __CPROVER_assume(__CPROVER_is_fresh(a_snapshot, a_len_before * item_sz));
        memcpy(a_snapshot, a_data_before, a_len_before * item_sz);
    }
    if (b_data_before != NULL && b_len_before > 0) {
        b_snapshot = malloc(b_len_before * item_sz);
        __CPROVER_assume(b_snapshot != NULL);
        __CPROVER_assume(__CPROVER_is_fresh(b_snapshot, b_len_before * item_sz));
        memcpy(b_snapshot, b_data_before, b_len_before * item_sz);
    }

    /* function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* post‑conditions */

    /* 1. validity predicates */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* 2. length / capacity invariants (they are swapped) */
    assert(list_a.length == b_len_before);
    assert(list_b.length == a_len_before);
    assert(list_a.current_size == b_cur_before);
    assert(list_b.current_size == a_cur_before);
    assert(list_a.item_size == list_b.item_size);
    assert(list_a.alloc == list_b.alloc);
    assert(list_a.data == b_data_before);
    assert(list_b.data == a_data_before);

    /* 3. frame conditions – underlying element data unchanged */
    if (b_data_before != NULL && b_len_before > 0) {
        assert(memcmp(b_data_before, a_snapshot, b_len_before * item_sz) == 0);
    }
    if (a_data_before != NULL && a_len_before > 0) {
        assert(memcmp(a_data_before, b_snapshot, a_len_before * item_sz) == 0);
    }

    free(a_snapshot);
    free(b_snapshot);
}
