#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* data structure */
    struct aws_array_list list;

    /* parameters */
    size_t index;
    void *val;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* non-deterministic value for val */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* non-deterministic index */
    __CPROVER_assume(index < list.length); // Ensure index is within bounds for setting

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size, &old_byte);

    /* perform operation under verification */
    int result = aws_array_list_set_at(&list, val, index);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length); // Length should not change on successful set
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size), (uint8_t *)val, list.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_byte_from_buffer_matches(list.data, &old_byte);
    }

    /* unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
}
