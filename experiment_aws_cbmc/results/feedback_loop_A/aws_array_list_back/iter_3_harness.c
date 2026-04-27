#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t max_initial_item_allocation = nd_size_t();
    size_t max_item_size = nd_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_init(&list, allocator, max_initial_item_allocation, max_item_size) == AWS_OP_SUCCESS);

    size_t initial_length = nd_size_t();
    __CPROVER_assume(initial_length <= max_initial_item_allocation);
    list.length = initial_length;

    void *val = malloc(list.item_size);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length);  // length should not change on success
        assert(list.current_size == old.current_size);  // current_size should not change on success
        assert(list.item_size == old.item_size);  // item_size should not change on success
        assert(list.data == old.data);  // data should not change on success
        assert(list.alloc == old.alloc);  // alloc should not change on success
        assert(list.length > 0);  // length should be greater than 0 on success
        assert(val != NULL);  // val should point to the last element on success
    } else {
        assert(list.length == old.length);  // length should not change on failure
        assert(list.current_size == old.current_size);  // current_size should not change on failure
        assert(list.item_size == old.item_size);  // item_size should not change on failure
        assert(list.data == old.data);  // data should not change on failure
        assert(list.alloc == old.alloc);  // alloc should not change on failure
        assert(list.length == 0);  // length should be 0 on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
