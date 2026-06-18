#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;
    size_t index = nondet_size_t();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    if (list.current_size > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    }

    struct aws_array_list old = list;

    size_t snapshot_size = old.current_size == 0 ? 1 : old.current_size;
    uint8_t *old_data = malloc(snapshot_size);
    __CPROVER_assume(old_data != NULL);

    if (old.current_size > 0) {
        __builtin_memcpy(old_data, old.data, old.current_size);
    }

    int result = aws_array_list_erase(&list, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (index >= old.length) {
        assert(result == AWS_OP_ERR);

        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        size_t unchanged_offset = nondet_size_t();
        if (unchanged_offset < old.current_size) {
            assert(((uint8_t *)list.data)[unchanged_offset] == old_data[unchanged_offset]);
        }
    } else {
        assert(result == AWS_OP_SUCCESS);

        assert(old.length > 0);
        assert(list.length == old.length - 1);

        size_t erased_byte_offset = index * old.item_size;
        size_t new_used_bytes = list.length * list.item_size;

        size_t prefix_offset = nondet_size_t();
        if (prefix_offset < erased_byte_offset) {
            assert(((uint8_t *)list.data)[prefix_offset] == old_data[prefix_offset]);
        }

        size_t shifted_offset = nondet_size_t();
        if (shifted_offset >= erased_byte_offset && shifted_offset < new_used_bytes) {
            assert(((uint8_t *)list.data)[shifted_offset] == old_data[shifted_offset + old.item_size]);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
