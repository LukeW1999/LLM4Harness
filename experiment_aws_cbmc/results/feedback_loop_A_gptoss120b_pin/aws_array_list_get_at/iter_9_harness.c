#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_array_list_get_at_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and initialize a bounded array list */
    struct aws_array_list list;
    list.alloc = allocator;
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    size_t init_capacity = nondet_size_t();
    __CPROVER_assume(init_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    aws_array_list_init(&list, allocator, init_capacity, list.item_size);
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Push a single element to make the list non‑empty */
    uint8_t *push_val = malloc(list.item_size);
    __CPROVER_assume(push_val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        push_val[i] = nondet_uint8_t();
    }
    int push_result = aws_array_list_push_back(&list, push_val);
    __CPROVER_assume(push_result == AWS_OP_SUCCESS);
    free(push_val);

    /* 3. Allocate a readable destination buffer `out` */
    uint8_t *out = malloc(list.item_size);
    __CPROVER_assume(out != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        out[i] = nondet_uint8_t();
    }

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Save old state for later comparison */
    struct aws_array_list old = list;

    /* 6. Call the function under test */
    int result = aws_array_list_get_at(&list, out, index);

    /* 7. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Successful read: index must be within bounds and list unchanged */
        assert(aws_array_list_is_valid(&list));
        assert(index < old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* Failure: list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    /* 8. Clean up */
    free(out);
    aws_array_list_clean_up(&list);
}
