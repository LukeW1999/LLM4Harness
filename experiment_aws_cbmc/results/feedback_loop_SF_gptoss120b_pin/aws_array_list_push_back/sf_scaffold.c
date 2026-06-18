#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic choice of dynamic or static list */
    bool is_dynamic = __CPROVER_nondet_bool();

    if (is_dynamic) {
        size_t initial_item_allocation = __CPROVER_nondet_size_t();
        size_t item_size = __CPROVER_nondet_size_t();

        __CPROVER_assume(item_size > 0);
        __CPROVER_assume(initial_item_allocation <= 1024);
        __CPROVER_assume(aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size) == AWS_OP_SUCCESS);
    } else {
        size_t item_count = __CPROVER_nondet_size_t();
        size_t item_size = __CPROVER_nondet_size_t();

        __CPROVER_assume(item_count > 0);
        __CPROVER_assume(item_size > 0);
        __CPROVER_assume(item_count <= 1024);
        void *raw = malloc(item_count * item_size);
        __CPROVER_assume(raw != NULL);
        __CPROVER_assume(aws_array_list_init_static(&list, raw, item_count, item_size) == AWS_OP_SUCCESS);
    }

    /* ensure the list is in a valid state before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocate and nondeterministically initialize the value to be pushed */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* PRE-CALL SNAPSHOT */
    size_t old_length = aws_array_list_length(&list);
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    uint8_t *old_contents = NULL;
    if (old_data != NULL && old_current_size > 0) {
        old_contents = malloc(old_current_size);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, old_data, old_current_size);
    }

    /* call the function under verification */
    int result = aws_array_list_push_back(&list, val);

    /* ASSERT_POSTCONDITIONS_HERE */
}
