#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_erase_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Keep sizes small to avoid huge allocations during verification */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);

    size_t max_items = nondet_size_t();
    __CPROVER_assume(max_items > 0);
    __CPROVER_assume(max_items <= 10);

    struct aws_array_list list;
    list.alloc = allocator;
    list.item_size = item_size;
    list.current_size = max_items * item_size;
    __CPROVER_assume(list.current_size > 0);               /* avoid zero allocation */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= max_items);

    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= max_items);   /* bound index to reasonable range */

    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    int result = aws_array_list_erase(&list, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (index >= old_length) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length - 1);
    } else {
        assert(list.length == old_length);
    }

    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(aws_array_list_is_valid(&list));
}
