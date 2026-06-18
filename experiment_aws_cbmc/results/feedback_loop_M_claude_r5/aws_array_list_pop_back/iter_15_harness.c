#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* 3. Provide a val buffer to receive the popped element */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant must always hold */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid after pop_back");

    /* Fields that must NOT change regardless of result */
    __CPROVER_assert(list.item_size == old_list.item_size, "item_size unchanged");
    __CPROVER_assert(list.alloc == old_list.alloc, "alloc unchanged");
    __CPROVER_assert(list.current_size == old_list.current_size, "current_size unchanged");

    if (result == AWS_OP_SUCCESS) {
        /* On success: length decreases by 1 */
        __CPROVER_assert(old_length > 0, "old_length > 0 on success");
        __CPROVER_assert(list.length == old_length - 1, "length decremented on success");
    } else {
        /* On failure: list was empty */
        __CPROVER_assert(old_length == 0, "old_length == 0 on failure");
        __CPROVER_assert(list.length == old_list.length, "length unchanged on failure");
    }
}
