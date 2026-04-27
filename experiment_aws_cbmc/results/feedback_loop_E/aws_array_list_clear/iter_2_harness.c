#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clear_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    size_t old_length = list.length;
    void *old_data = list.data;

    /* perform operation under verification */
    aws_array_list_clear(&list);

    /* assertions */
    /* Check that the length is reset to 0 */
    assert(list.length == 0);

    /* Check that other fields remain unchanged */
    assert(list.alloc == list.alloc);
    assert(list.item_size == list.item_size);
    assert(list.current_size == list.current_size);
    assert(list.data == old_data);

    /* Check that the array list is still valid */
    assert(aws_array_list_is_valid(&list));

    /* Check that the memory pointed by data is not altered if it was not freed */
    if (old_data && old_length > 0) {
        unsigned char *old_data_bytes = (unsigned char *)old_data;
        for (size_t i = 0; i < old_length * list.item_size; i++) {
            assert(old_data_bytes[i] == old_data_bytes[i]);
        }
    }
}
