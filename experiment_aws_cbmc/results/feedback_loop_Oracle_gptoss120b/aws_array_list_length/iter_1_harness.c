#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_length_harness(void) {
    /* Non-deterministic fields for the list */
    struct aws_array_list list;
    size_t item_size;
    size_t length;
    size_t current_size;
    bool have_alloc;

    /* Ensure reasonable, non‑overflowing values */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(current_size <= 1024 * 1024);
    __CPROVER_assume(length <= current_size / item_size);

    /* Allocate backing storage if needed */
    uint8_t *data = NULL;
    if (current_size > 0) {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    /* Set allocator (dynamic vs static mode) */
    if (have_alloc) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    /* Populate the list structure */
    list.data = data;
    list.current_size = current_size;
    list.length = length;
    list.item_size = item_size;

    /* Satisfy the precondition of aws_array_list_length */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot the initial state for frame condition checks */
    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (data) {
        old_data = malloc(current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, data, current_size);
    }

    /* Invoke the function under test */
    size_t ret = aws_array_list_length(&list);

    /* 1. Return value / error code correctness */
    assert(ret == old_list.length);

    /* 2. Output buffer length/capacity invariants (list unchanged) */
    assert(memcmp(&list, &old_list, sizeof(list)) == 0);

    /* 3. Memory frame condition (underlying buffer unchanged) */
    if (data) {
        assert(memcmp(data, old_data, current_size) == 0);
    }

    return 0;
}
