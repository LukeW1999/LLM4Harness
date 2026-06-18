#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    struct aws_array_list old;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    old = list;

    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, item_size));
    for (size_t i = 0; i < item_size; i++) {
        uint8_t byte;
        val[i] = byte;
    }

    size_t index;
    __CPROVER_assume(index < SIZE_MAX);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        for (size_t i = 0; i < list.item_size; i++) {
            assert(((uint8_t *)list.data)[index * list.item_size + i] == val[i]);
        }
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
}
