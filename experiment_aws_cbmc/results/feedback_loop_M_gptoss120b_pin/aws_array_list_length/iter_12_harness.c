#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

size_t nondet_size_t(void);

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *val_buf = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val_buf != NULL || list.item_size == 0);

    int result = aws_array_list_set_at(&list, val_buf, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        uint8_t *dest = (uint8_t *)list.data + index * list.item_size;
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(dest[i] == val_buf[i]);
        }

        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    if (val_buf != NULL) {
        aws_mem_release(allocator, val_buf);
    }
}
