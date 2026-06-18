#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list = {0};
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure the list has a non‑zero item size for allocation */
    __CPROVER_assume(list.item_size > 0);

    uint8_t *val = aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = (size_t)nondet_uint64_t();

    int result = aws_array_list_set_at(&list, val, index);

    /* The function should preserve allocator and item size */
    __CPROVER_assert(list.alloc == allocator, "allocator unchanged");
    __CPROVER_assert(list.item_size > 0, "item size unchanged");

    /* The list must remain valid after the operation */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    aws_mem_release(allocator, val);
}
