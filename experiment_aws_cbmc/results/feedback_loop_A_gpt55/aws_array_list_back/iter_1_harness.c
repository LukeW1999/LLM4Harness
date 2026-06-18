#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    struct aws_array_list old = list;

    struct store_byte_from_buffer old_byte;
    if (old.current_size > 0) {
        save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_back(&list, val);

    if (old.length > 0) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        if (old.length > 0) {
            size_t last_item_offset = old.item_size * (old.length - 1);
            assert_bytes_match((const uint8_t *)val, (const uint8_t *)old.data + last_item_offset, old.item_size);
        }
    } else {
        assert(old.length == 0);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (old.current_size > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
