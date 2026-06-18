#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = val_buf;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert(!memcmp((uint8_t *)list.data + index * list.item_size,
                       val_buf,
                       list.item_size));

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);

        if (old.current_size > 0) {
            assert(!memcmp((uint8_t *)list.data,
                           (uint8_t *)old.data,
                           old.current_size));
        }
    }
}
