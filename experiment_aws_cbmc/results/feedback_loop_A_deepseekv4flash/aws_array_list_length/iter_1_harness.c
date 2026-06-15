#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index < SIZE_MAX / list.item_size);
    if (list.alloc == NULL) {
        __CPROVER_assume(index < list.current_size / list.item_size);
    }
    __CPROVER_assume(index < SIZE_MAX);

    int result = aws_array_list_set_at(&list, (const void *)val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size), val, list.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }
}
