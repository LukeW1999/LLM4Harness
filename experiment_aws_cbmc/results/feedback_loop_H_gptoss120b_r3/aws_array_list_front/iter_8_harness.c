#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    size_t initial_capacity = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(initial_capacity > 0);
    __CPROVER_assume(item_size > 0);
    aws_array_list_init(&list, allocator, initial_capacity, item_size);

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set a nondeterministic length that does not exceed the allocated capacity */
    size_t max_len = list.current_size / list.item_size;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);
    list.length = len;

    struct aws_array_list old = list;

    void *front = aws_array_list_front(&list);

    if (list.length > 0) {
        assert(front == list.data);
    } else {
        assert(front == NULL);
    }

    /* The list must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
