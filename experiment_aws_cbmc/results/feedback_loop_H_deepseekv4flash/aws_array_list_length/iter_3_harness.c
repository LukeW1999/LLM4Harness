#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_length_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t len_val;
    size_t *len = &len_val;

    /* Ensure preconditions for aws_array_list_is_valid */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(len != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(len, sizeof(size_t)));

    /* Save old state for comparison */
    struct aws_array_list old = list;

    /* Call the function */
    int result = aws_array_list_length(&list, len);

    /* Assert postconditions */
    __CPROVER_assert(result == AWS_OP_SUCCESS, "Function must succeed");
    __CPROVER_assert(*len == list.length, "Length output equals list.length");
    __CPROVER_assert(list.length == old.length, "Length unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "Current size unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "Item size unchanged");
    __CPROVER_assert(list.alloc == old.alloc, "Allocator unchanged");
    __CPROVER_assert(list.data == old.data, "Data pointer unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "List remains valid");
}
