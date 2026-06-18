#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and initialize the array list with bounded parameters */
    struct aws_array_list list;

    /* Use nondet sizes within bounds */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the list properly */
    if (nondet_bool()) {
        /* Dynamic list */
        int rc = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
        __CPROVER_assume(rc == AWS_OP_SUCCESS);
    } else {
        /* Static list with a backing array */
        size_t buf_size;
        __CPROVER_assume(buf_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
        __CPROVER_assume(buf_size >= item_size);
        void *raw_array = malloc(buf_size);
        __CPROVER_assume(raw_array != NULL);
        aws_array_list_init_static(&list, raw_array, buf_size / item_size, item_size);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null */
        assert(list.data != NULL);
    } else {
        /* On failure: length unchanged */
        assert(list.length == old_length);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
    }
}
