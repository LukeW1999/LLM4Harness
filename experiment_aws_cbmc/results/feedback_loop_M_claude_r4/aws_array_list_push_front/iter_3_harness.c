#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    /* Initialize the list with bounded parameters */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Initialize the array list */
    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set a nondet length bounded by current capacity */
    size_t nondet_length;
    size_t max_length = list.current_size / list.item_size;
    __CPROVER_assume(nondet_length <= max_length);
    list.length = nondet_length;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Create val buffer */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size >= list.length * list.item_size);
        if (list.data != NULL) {
            assert(memcmp(list.data, val, list.item_size) == 0);
        }
    } else {
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
    }

    assert(aws_array_list_is_valid(&list));
}
