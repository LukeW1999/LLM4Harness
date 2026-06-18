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
    /* 1. Declare and initialize the array list */
    struct aws_array_list list;

    /* Non-deterministic item_size and initial_item_count */
    size_t item_size;
    size_t initial_item_count;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Initialize the list dynamically */
    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_count, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministically set the length to something valid */
    size_t initial_length;
    __CPROVER_assume(initial_length <= initial_item_count);
    list.length = initial_length;

    /* 2. Create a val buffer of item_size bytes */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state for postcondition checks */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
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

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
