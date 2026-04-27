#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    void *val;
    size_t item_size;
    size_t initial_item_allocation;

    /* assumptions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_bounded(&list, initial_item_allocation, item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* non-deterministic input value */
    val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* perform operation under verification */
    int result = aws_array_list_push_front(&list, val);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.current_size >= (old.length + 1) * item_size);
        assert(memcmp(list.data, val, item_size) == 0);
        if (old.length > 0) {
            assert(memcmp((uint8_t *)list.data + item_size, old.data, old.length * item_size) == 0);
        }
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
}
