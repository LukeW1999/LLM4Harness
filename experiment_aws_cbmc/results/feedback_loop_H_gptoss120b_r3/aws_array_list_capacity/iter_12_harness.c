#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t max_items = MAX_INITIAL_ITEM_ALLOCATION;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    aws_array_list_init(&list, allocator, max_items, item_size);

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_items);
    list.length = len;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, item_size);
        __CPROVER_assume(val != NULL);
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(index < (max_items * 2));

    struct aws_array_list old = list;
    size_t old_length = old.length;
    size_t old_current_size = old.current_size;
    void *old_data = old.data;

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        if (item_size > 0 && list.data) {
            for (size_t i = 0; i < item_size; ++i) {
                assert(((uint8_t *)list.data)[index * item_size + i] == val[i]);
            }
        }
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
        assert(list.current_size >= old_current_size);
        assert(list.current_size >= (index + 1) * item_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    if (val) {
        aws_mem_release(allocator, val);
    }

    aws_array_list_clean_up(&list);
}
