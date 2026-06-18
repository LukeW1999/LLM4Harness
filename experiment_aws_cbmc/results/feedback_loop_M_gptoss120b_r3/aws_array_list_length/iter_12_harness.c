#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 5
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 32
#endif

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t init_cap = (size_t)nondet_uint64_t();
    __CPROVER_assume(init_cap <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_sz = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);

    if (aws_array_list_init_dynamic(&list, allocator, init_cap, item_sz) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val_buf[MAX_ITEM_SIZE];
    uint8_t *val = val_buf;

    size_t index = (size_t)nondet_uint64_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            __CPROVER_assert(list.length == index + 1,
                             "length updated when index beyond old length");
        } else {
            __CPROVER_assert(list.length == old.length,
                             "length unchanged when index within old length");
        }

        for (size_t i = 0; i < list.item_size; ++i) {
            __CPROVER_assert(((uint8_t *)list.data)[index * list.item_size + i] == val[i],
                             "value stored correctly");
        }

        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    } else {
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size,
                         "current_size unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data pointer unchanged on failure");
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    aws_array_list_clean_up(&list);
}
