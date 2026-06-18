#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.data != NULL);
        if (index < aws_array_list_length(&list)) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               (const uint8_t *)val, list.item_size);
        }
        if (index >= old.length) {
            assert(list.length == index + 1);
        }
    } else {
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    if (list.current_size == old.current_size) {
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
}
