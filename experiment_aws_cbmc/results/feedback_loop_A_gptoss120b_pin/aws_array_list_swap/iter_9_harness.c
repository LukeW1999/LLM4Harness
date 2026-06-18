#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    size_t data_bytes = list.length * list.item_size;

    struct aws_array_list old = list;
    uint8_t *old_bytes = 0;
    if (data_bytes > 0) {
        old_bytes = malloc(data_bytes);
        __CPROVER_assume(old_bytes != 0);
        memcpy(old_bytes, list.data, data_bytes);
    }

    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer unchanged");

    if (a == b) {
        if (data_bytes > 0) {
            __CPROVER_assert(memcmp(list.data, old_bytes, data_bytes) == 0,
                             "no change when a==b");
        }
    } else {
        uint8_t *data = (uint8_t *)list.data;
        __CPROVER_assert(memcmp(data + a * list.item_size,
                                old_bytes + b * list.item_size,
                                list.item_size) == 0,
                         "a swapped with b");
        __CPROVER_assert(memcmp(data + b * list.item_size,
                                old_bytes + a * list.item_size,
                                list.item_size) == 0,
                         "b swapped with a");
        for (size_t i = 0; i < list.length; ++i) {
            if (i != a && i != b) {
                __CPROVER_assert(memcmp(data + i * list.item_size,
                                        old_bytes + i * list.item_size,
                                        list.item_size) == 0,
                                 "unchanged element");
            }
        }
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    free(old_bytes);
}
