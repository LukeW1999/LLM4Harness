#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;
    void *val;
    size_t index;

    // Initialize list with some arbitrary values
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_array_list_init_dynamic(&list, allocator, MAX_ITEM_ALLOC, MAX_ITEM_SIZE) == AWS_OP_SUCCESS);

    // Add elements to the list to make sure we can get an element
    for (size_t i = 0; i < MAX_ITEM_ALLOC; i++) {
        void *item = can_fail_malloc(MAX_ITEM_SIZE);
        __CPROVER_assume(item != NULL);
        aws_array_list_push_back(&list, item);
    }

    // Initialize val with a nondeterministic value
    val = can_fail_malloc(MAX_ITEM_SIZE);

    // Initialize index with a nondeterministic value within bounds
    index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    // Save old state of list
    struct aws_array_list old_list = list;
    void *old_val = val;

    int result = aws_array_list_get_at_ptr(&list, &val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(val != old_val); // *val should point to the element at index
        assert(*(void **)val == *(void **)(list.data + index * list.item_size));
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    } else {
        assert(val == old_val); // *val should remain unchanged
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
    aws_array_list_clean_up(&list);
}
