#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_swap_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    ensure_array_list_is_bounded(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* The list must contain at least one element for the indices to be valid */
    __CPROVER_assume(list.length > 0);

    /* 2. Non‑deterministic indices bounded to be within the list */
    size_t a = (size_t)nondet_uint64_t();
    __CPROVER_assume(a < list.length);
    size_t b = (size_t)nondet_uint64_t();
    __CPROVER_assume(b < list.length);

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* Save a copy of the whole data buffer */
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = (uint8_t *)aws_mem_acquire(allocator, list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Save the elements at positions a and b */
    uint8_t *old_a = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    uint8_t *old_b = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(old_a != NULL && old_b != NULL);
    memcpy(old_a,
           (const uint8_t *)list.data + a * list.item_size,
           list.item_size);
    memcpy(old_b,
           (const uint8_t *)list.data + b * list.item_size,
           list.item_size);

    /* 4. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Post‑condition: elements at a and b are swapped */
    assert(memcmp((const uint8_t *)list.data + a * list.item_size,
                  old_b,
                  list.item_size) == 0);
    assert(memcmp((const uint8_t *)list.data + b * list.item_size,
                  old_a,
                  list.item_size) == 0);

    /* 6. All other elements remain unchanged */
    for (size_t i = 0; i < list.length; ++i) {
        if (i != a && i != b) {
            assert(memcmp((const uint8_t *)list.data + i * list.item_size,
                          old_data + i * list.item_size,
                          list.item_size) == 0);
        }
    }

    /* 7. Fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up auxiliary allocations */
    if (old_data) {
        aws_mem_release(allocator, old_data);
    }
    aws_mem_release(allocator, old_a);
    aws_mem_release(allocator, old_b);
}
