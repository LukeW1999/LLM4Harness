#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet initialization of the list fields */
    list.alloc = nondet_pointer();
    __CPROVER_assume(list.alloc == NULL || list.alloc == allocator);
    list.item_size = nondet_size_t();
    list.length = nondet_size_t();
    list.current_size = nondet_size_t();
    list.data = nondet_pointer();

    /* consistency: if both alloc and data are non‑NULL, data must be a fresh block */
    __CPROVER_assume(!(list.alloc != NULL && list.data != NULL) ||
                     __CPROVER_is_fresh(list.data, list.current_size));

    aws_array_list_clean_up(&list);
    return 0;
}
