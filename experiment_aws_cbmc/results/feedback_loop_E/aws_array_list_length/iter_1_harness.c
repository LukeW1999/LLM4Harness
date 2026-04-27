#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    size_t item_size;
    size_t initial_item_allocation;
    size_t len;

    /* assumptions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &len));

    /* perform operation under verification */
    uint8_t *raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    /* save old state */
    struct aws_array_list old_list = list;

    /* call function under test */
    size_t result = aws_array_list_length(&list);

    /* assertions */
    assert(aws_array_list_is_valid(&list));

    /* Check that length is returned correctly */
    assert(result == list.length);

    /* Check unchanged fields */
    assert(list.alloc == old_list.alloc);
    assert(list.data == old_list.data);
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
}
