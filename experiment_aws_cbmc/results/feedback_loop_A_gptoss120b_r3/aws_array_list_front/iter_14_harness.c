#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>
#include <stdint.h>

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
    void *old_data_copy = NULL;
    if (old.data && old.current_size > 0) {
        old_data_copy = allocator->mem_acquire(allocator, old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        for (size_t i = 0; i < old.current_size; ++i) {
            ((uint8_t *)old_data_copy)[i] = ((uint8_t *)old.data)[i];
        }
    }

    void *front = aws_array_list_front(&list);

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    if (list.length > 0) {
        __CPROVER_assert(front == list.data, "front points to data");
        for (size_t i = 0; i < list.item_size; ++i) {
            __CPROVER_assert(((uint8_t *)front)[i] == ((uint8_t *)list.data)[i],
                             "front element matches");
        }
    } else {
        __CPROVER_assert(front == NULL, "front is NULL when empty");
    }

    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer unchanged");
    if (old.data && old.current_size > 0) {
        for (size_t i = 0; i < old.current_size; ++i) {
            __CPROVER_assert(((uint8_t *)list.data)[i] == ((uint8_t *)old_data_copy)[i],
                             "data unchanged");
        }
    }

    if (old_data_copy) {
        allocator->mem_release(allocator, old_data_copy);
    }
}
