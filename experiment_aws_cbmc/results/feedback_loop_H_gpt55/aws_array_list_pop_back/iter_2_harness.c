#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = nondet_bool() ? allocator : NULL;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_pop_back(&list);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "pop_back returns success or error");

    if (old.length == 0) {
        __CPROVER_assert(result == AWS_OP_ERR, "pop_back on empty list fails");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
    } else {
        __CPROVER_assert(result == AWS_OP_SUCCESS, "pop_back on non-empty list succeeds");
        __CPROVER_assert(list.length == old.length - 1, "length decremented on success");
    }

    __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current size unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item size unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
