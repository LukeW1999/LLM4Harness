#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* allocator must be non‑NULL for a valid list */
    list.alloc = allocator;

    /* item_size must be > 0 */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* current_size must be a multiple of item_size */
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size % list.item_size == 0);

    /* length must not exceed current_size */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);

    /* data may be NULL or a fresh block of size current_size */
    list.data = nondet_pointer();
    __CPROVER_assume(!(list.data != NULL) ||
                     __CPROVER_is_fresh(list.data, list.current_size));

    aws_array_list_clean_up(&list);
}
