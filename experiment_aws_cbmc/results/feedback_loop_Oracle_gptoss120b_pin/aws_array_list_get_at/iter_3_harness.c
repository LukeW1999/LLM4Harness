#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size < 1024);
    size_t max_items = nondet_size_t();
    __CPROVER_assume(max_items > 0 && max_items < 64);

    if (aws_array_list_init_dynamic(&list, allocator, max_items, item_size)) {
        return;
    }

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= max_items);
    list.length = length;

    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < list.length);

    void *out = aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(out != NULL);

    int rc = aws_array_list_get_at(&list, idx, out);
    __CPROVER_assume(rc == AWS_OP_SUCCESS);

    aws_array_list_clean_up(&list);
    aws_mem_release(allocator, out);
}
