#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ITEM_SIZE 256
#define MAX_INITIAL_ITEM_ALLOCATION 1024

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    size_t initial_capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    aws_array_list_init(&list, allocator, initial_capacity, item_size);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t index = (size_t)nondet_uint64_t();
    __CPROVER_assume(index <= initial_capacity);

    uint8_t *val_buf = NULL;
    if (item_size > 0) {
        val_buf = (uint8_t *)malloc(item_size);
        __CPROVER_assume(val_buf != NULL);
    }

    int result = aws_array_list_set_at(&list, val_buf, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(memcmp((uint8_t *)list.data + index * list.item_size, val_buf, list.item_size) == 0);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    aws_array_list_clean_up(&list);
}
