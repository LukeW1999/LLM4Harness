#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_is_bounded(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0);

    /* Save a copy of the entire list data for later comparison */
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = (uint8_t *)aws_mem_acquire(allocator, list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Save a copy of the first element */
    uint8_t *expected = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(expected != NULL);
    memcpy(expected, (const uint8_t *)list.data, list.item_size);

    /* Buffer to receive the front element */
    uint8_t *out = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(out != NULL);

    /* Preserve the list structure */
    struct aws_array_list old = list;

    /* Call the function under test */
    int ret = aws_array_list_front(&list, out);
    assert(ret == 0);

    /* Verify the returned element matches the original first element */
    assert(memcmp(out, expected, list.item_size) == 0);

    /* Verify the list itself has not been modified */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);
    if (old_data) {
        assert(memcmp(list.data, old_data, list.current_size) == 0);
    }

    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    if (old_data) {
        aws_mem_release(allocator, old_data);
    }
    aws_mem_release(allocator, out);
    aws_mem_release(allocator, expected);
}
