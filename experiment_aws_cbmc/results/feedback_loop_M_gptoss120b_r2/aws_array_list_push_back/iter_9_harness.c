#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

extern bool aws_array_list_is_bounded(
    const struct aws_array_list *list,
    size_t max_initial_allocation,
    size_t max_item_size);

void aws_array_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    __CPROVER_assume(list.item_size > 0);
    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    int result = aws_array_list_push_back(&list, (const void *)val);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(list.length == old.length + 1,
                         "length increased by one");
        for (size_t i = 0; i < list.item_size; ++i) {
            __CPROVER_assert(((uint8_t *)list.data)[old.length * list.item_size + i] == val[i],
                             "pushed element matches");
        }
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    } else {
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data pointer unchanged on failure");
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    aws_mem_release(allocator, val);
}
