#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    struct aws_allocator *allocator = aws_default_allocator();
    if (nondet_bool()) {
        list.alloc = allocator;
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length < (size_t)-1);
    __CPROVER_assume(list.length + 1 <= ((size_t)-1) / list.item_size);

    uint8_t val[MAX_ITEM_SIZE];
    uint8_t val_before[MAX_ITEM_SIZE];

    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }

    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    for (size_t i = 0; i < list.item_size; ++i) {
        val_before[i] = val[i];
    }

    struct aws_array_list old = list;

    int result = aws_array_list_push_front(&list, val);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "push_front returns success or error");

    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid after push_front");
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assert(list.alloc == old.alloc, "allocator is unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item size is unchanged");

    for (size_t i = 0; i < old.item_size; ++i) {
        __CPROVER_assert(val[i] == val_before[i], "value buffer is unchanged");
    }

    if (result == AWS_OP_SUCCESS) {
        size_t required_size = (old.length + 1) * old.item_size;

        __CPROVER_assert(list.length == old.length + 1, "length is incremented on success");
        __CPROVER_assert(list.current_size >= required_size, "capacity is sufficient on success");

        for (size_t i = 0; i < old.item_size; ++i) {
            __CPROVER_assert(((uint8_t *)list.data)[i] == val_before[i], "new item is at front");
        }

        if (old.alloc == NULL || old.current_size >= required_size) {
            __CPROVER_assert(list.current_size == old.current_size, "capacity is unchanged when no allocation is needed");
            __CPROVER_assert(list.data == old.data, "data pointer is unchanged when no allocation is needed");
        } else {
            __CPROVER_assert(list.current_size >= old.current_size, "capacity does not shrink");
        }
    } else {
        __CPROVER_assert(list.length == old.length, "length is unchanged on error");
        __CPROVER_assert(list.current_size == old.current_size, "capacity is unchanged on error");
        __CPROVER_assert(list.data == old.data, "data pointer is unchanged on error");
    }
}
