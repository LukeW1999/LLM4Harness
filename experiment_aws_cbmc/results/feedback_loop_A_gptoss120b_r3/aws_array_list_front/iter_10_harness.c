#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <string.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.data && old.current_size > 0) {
        old_data_copy = (uint8_t *)aws_mem_acquire(allocator, old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.current_size);
    }

    void *front = aws_array_list_front(&list);

    assert(aws_array_list_is_valid(&list));

    if (list.length > 0) {
        assert(front == list.data);
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)front)[i] == ((uint8_t *)list.data)[i]);
        }
    } else {
        assert(front == NULL);
    }

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (old.data && old.current_size > 0) {
        for (size_t i = 0; i < old.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data_copy[i]);
        }
    }

    aws_mem_release(allocator, old_data_copy);
}
