#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.length > 0 && old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.data != NULL);
        assert(val != NULL);
        assert_bytes_match((uint8_t *)list.data + ((list.length - 1) * list.item_size),
                           val,
                           list.item_size);
        assert(list.length == old.length + 1);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size >= old.current_size);

        if (list.current_size > 0 && old.data != NULL && list.data != NULL) {
            size_t i;
            for (i = 0; i < list.current_size; ++i) {
                bool in_new_elem = (i >= (old.length * list.item_size)) &&
                                   (i < (list.length * list.item_size));
                if (!in_new_elem) {
                    assert(((uint8_t *)list.data)[i] == ((uint8_t *)old.data)[i]);
                }
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        if (list.current_size > 0 && old.data != NULL && list.data != NULL) {
            size_t i;
            for (i = 0; i < list.current_size; ++i) {
                assert(((uint8_t *)list.data)[i] == ((uint8_t *)old.data)[i]);
            }
        }
    }

    assert(aws_array_list_is_valid(&list));

    if (val) {
        aws_mem_release(allocator, val);
    }
}
