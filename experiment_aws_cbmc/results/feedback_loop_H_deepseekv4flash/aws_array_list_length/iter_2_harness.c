#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_length_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t *len = malloc(sizeof(size_t));

    /* Assume valid list and writable len */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(len != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(len, sizeof(size_t)));

    /* Save old state */
    struct aws_array_list old = list;

    /* Call function */
    int result = aws_array_list_length(&list, len);

    /* Assert postconditions */
    assert(result == AWS_OP_SUCCESS);
    assert(*len == list.length);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(aws_array_list_is_valid(&list));
}
