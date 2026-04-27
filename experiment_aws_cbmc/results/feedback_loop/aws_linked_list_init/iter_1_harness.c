#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_init_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));

    /* 3. Allocate pointer members */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)&list, sizeof(struct aws_linked_list), &old_byte);

    /* 6. Assume function-specific preconditions */
    // No specific preconditions for aws_linked_list_init

    /* 7. Call function under test */
    aws_linked_list_init(&list);

    /* 8. Assert postconditions (both branches) */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
    assert_byte_from_buffer_matches((uint8_t *)&list, &old_byte);
}
