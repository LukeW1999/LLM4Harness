#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Preconditions: list is valid, has allocated data, and item_size > 0 */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Prepare val: allocate memory of item_size bytes, assume non-null */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Call function */
    int result = aws_array_list_get_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(index < list.length);
        /* Check that the element was copied correctly */
        uint8_t *src = (uint8_t *)list.data + index * list.item_size;
        for (size_t i = 0; i < list.item_size; i++) {
            assert(src[i] == ((uint8_t *)val)[i]);
        }
        /* List unchanged */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    } else {
        assert(index >= list.length);
        /* List unchanged */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
}
