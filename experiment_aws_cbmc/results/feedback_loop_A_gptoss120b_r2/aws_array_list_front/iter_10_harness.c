#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_sz = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= 1024);
    size_t initial_capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(initial_capacity <= 64);
    aws_array_list_init(&list, allocator, initial_capacity, item_sz);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    unsigned char *val = (unsigned char *)aws_mem_acquire(allocator, item_sz);
    __CPROVER_assume(val != NULL);
    if (nondet_bool()) {
        aws_array_list_push_back(&list, val);
    }
    aws_mem_release(allocator, val);

    struct aws_array_list old = list;

    const void *front = aws_array_list_front(&list);

    if (list.length > 0) {
        assert(front != NULL);
        assert(front == list.data);
    } else {
        assert(front == NULL);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    assert(list.length == old.length);

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
