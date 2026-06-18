#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    /* nondeterministic fields with bounds to keep verification tractable */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size <= 64);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= 64);

    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= 64);

    if (list.length > 0) {
        __CPROVER_assume(list.item_size > 0);
        __CPROVER_assume(list.current_size >= list.length * list.item_size);
        list.data = aws_mem_acquire(allocator, list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        __CPROVER_assume(list.current_size == 0);
        list.data = NULL;
    }

    /* assume the list is in a valid state before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of pre‑state */
    struct aws_allocator *old_alloc        = list.alloc;
    void                 *old_data         = list.data;
    size_t                old_current_size = list.current_size;
    size_t                old_length        = list.length;
    size_t                old_item_size     = list.item_size;

    /* call the function under verification */
    aws_array_list_clean_up(&list);

    /* post‑condition assertions */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
}
