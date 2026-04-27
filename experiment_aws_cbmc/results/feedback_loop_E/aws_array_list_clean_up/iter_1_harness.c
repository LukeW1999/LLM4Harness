#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;

    /* perform operation under verification */
    aws_array_list_clean_up(&list);

    /* assertions */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == NULL);
    assert(list.item_size == 0);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.data == NULL);

    /* unchanged fields */
    assert(list.alloc == old.alloc); /* Assuming alloc is not modified if data is freed */
}
