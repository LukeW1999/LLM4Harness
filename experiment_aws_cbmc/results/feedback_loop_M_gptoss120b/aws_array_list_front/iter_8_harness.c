#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_swap_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_is_bounded(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0);

    size_t a = nondet_uint64_t();
    __CPROVER_assume(a < list.length);
    size_t b = nondet_uint64_t();
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = (uint8_t *)aws_mem_acquire(allocator, list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    uint8_t *old_a = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    uint8_t *old_b = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(old_a != NULL && old_b != NULL);
    memcpy(old_a,
           (const uint8_t *)list.data + a * list.item_size,
           list.item_size);
    memcpy(old_b,
           (const uint8_t *)list.data + b * list.item_size,
           list.item_size);

    aws_array_list_swap(&list, a, b);

    assert(memcmp((const uint8_t *)list.data + a * list.item_size,
                  old_b,
                  list.item_size) == 0);
    assert(memcmp((const uint8_t *)list.data + b * list.item_size,
                  old_a,
                  list.item_size) == 0);

    for (size_t i = 0; i < list.length; ++i) {
        if (i != a && i != b) {
            assert(memcmp((const uint8_t *)list.data + i * list.item_size,
                          old_data + i * list.item_size,
                          list.item_size) == 0);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));

    if (old_data) {
        aws_mem_release(allocator, old_data);
    }
    aws_mem_release(allocator, old_a);
    aws_mem_release(allocator, old_b);
}
