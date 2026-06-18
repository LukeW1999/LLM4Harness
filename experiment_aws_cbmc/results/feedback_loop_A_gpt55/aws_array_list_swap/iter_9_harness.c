#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length > 1);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    __CPROVER_assume(a <= SIZE_MAX / old.item_size);
    __CPROVER_assume(b <= SIZE_MAX / old.item_size);

    size_t offset_a = a * old.item_size;
    size_t offset_b = b * old.item_size;

    __CPROVER_assume(old.item_size <= old.current_size);
    __CPROVER_assume(offset_a <= old.current_size - old.item_size);
    __CPROVER_assume(offset_b <= old.current_size - old.item_size);

    aws_array_list_swap(&list, a, b);

    __CPROVER_assert(list.alloc == old.alloc, "allocator is unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size is unchanged");
    __CPROVER_assert(list.length == old.length, "length is unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size is unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer is unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
