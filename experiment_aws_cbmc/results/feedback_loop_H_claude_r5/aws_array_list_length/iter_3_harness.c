#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;

    /* 3. Call function under test */
    size_t result = aws_array_list_length(&list);

    /* 4. Assert postconditions */

    /* Validity invariant always holds */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid after call");

    /* The returned length matches the internal length field */
    __CPROVER_assert(result == old_length, "result equals old length");

    /* The list is not modified */
    __CPROVER_assert(list.length == old_length, "list length unchanged");
}
