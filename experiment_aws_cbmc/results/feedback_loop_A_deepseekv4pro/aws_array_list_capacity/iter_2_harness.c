#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    size_t index;

    struct aws_array_list old = list;
    size_t old_length = aws_array_list_length(&list);

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(index < aws_array_list_length(&list));
        const uint8_t *element = (const uint8_t *)list.data + index * list.item_size;
        assert_bytes_match(element, val, list.item_size);
        if (index >= old_length) {
            assert(aws_array_list_length(&list) == index + 1);
        } else {
            assert(aws_array_list_length(&list) == old_length);
        }
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }

    free(val);
}
