#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    if (list.item_size != 0) {
        __CPROVER_assume(index <= SIZE_MAX / list.item_size);
    }

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.data && old.current_size > 0) {
        old_data_copy = aws_mem_acquire(allocator, old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.current_size);
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (uint8_t *)val,
                           list.item_size);

        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        for (size_t i = 0; i < old.length; ++i) {
            if (i == index) {
                continue;
            }
            assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                               (uint8_t *)old_data_copy + (i * old.item_size),
                               list.item_size);
        }
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data && old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old_data_copy,
                               list.current_size);
        }
        assert(aws_array_list_is_valid(&list));
    }

    aws_mem_release(allocator, val);
    aws_mem_release(allocator, old_data_copy);
}
