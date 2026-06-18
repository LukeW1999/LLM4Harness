#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = (const void *)val_buf;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
