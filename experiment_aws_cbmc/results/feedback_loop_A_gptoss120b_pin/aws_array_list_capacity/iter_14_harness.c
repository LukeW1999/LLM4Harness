#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity > 0 && initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    aws_array_list_init(&list, allocator, initial_capacity, item_size);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);

    size_t index = nondet_size_t();

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        for (size_t i = 0; i < list.item_size; ++i) {
            uint8_t stored = *((uint8_t *)list.data + (index * list.item_size) + i);
            uint8_t expected = *((uint8_t *)val + i);
            assert(stored == expected);
        }
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    aws_mem_release(allocator, val);
    aws_array_list_clean_up(&list);
}
