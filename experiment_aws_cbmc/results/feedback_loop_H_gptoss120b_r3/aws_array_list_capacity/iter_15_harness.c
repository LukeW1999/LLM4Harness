#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_capacity_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t max_items = nondet_size_t();
    __CPROVER_assume(max_items <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    aws_array_list_init(&list, allocator, max_items, item_size);

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_items);
    list.length = len;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    void *old_data = old.data;
    size_t old_current_size = old.current_size;
    size_t old_length = old.length;
    size_t old_item_size = old.item_size;
    struct aws_allocator *old_alloc = old.alloc;

    size_t cap = aws_array_list_capacity(&list);

    if (list.item_size > 0) {
        assert(cap == list.current_size / list.item_size);
    } else {
        assert(cap == 0);
    }

    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    aws_array_list_clean_up(&list);
}
