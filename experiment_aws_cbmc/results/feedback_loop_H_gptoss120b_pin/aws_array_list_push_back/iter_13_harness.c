#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 256
#define MAX_INITIAL_ITEM_ALLOCATION 10

void aws_array_list_set_at_harness() {
    struct aws_array_list list = {0};
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    list.length = nondet_size_t();
    list.current_size = nondet_size_t();

    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t item_sz = list.item_size;
    uint8_t *val = (uint8_t *)aws_mem_acquire(list.alloc, item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    size_t index = nondet_size_t();

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(
            memcmp((uint8_t *)list.data + (index * list.item_size),
                   val,
                   list.item_size) == 0,
            "value stored correctly");
        if (index >= old.length) {
            __CPROVER_assert(list.length == index + 1, "length updated");
        } else {
            __CPROVER_assert(list.length == old.length, "length unchanged");
        }
        __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged");
        __CPROVER_assert(list.item_size == old.item_size, "item size unchanged");
        __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
    } else {
        __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item size unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current size unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data pointer unchanged on failure");
        __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid on failure");
    }

    aws_mem_release(list.alloc, val);
}
