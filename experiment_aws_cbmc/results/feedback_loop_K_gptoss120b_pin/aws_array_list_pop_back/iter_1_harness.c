#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to obtain a nondeterministic size */
size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* --- Set up a nondeterministic but well‑formed list ----------------------------------- */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity = nondet_size_t();               /* number of items that can be stored */
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 1024);              /* keep allocation bounded */

    size_t current_size = item_size * capacity;
    __CPROVER_assume(current_size / item_size == capacity);   /* no overflow */

    uint8_t *buffer = malloc(current_size);
    __CPROVER_assume(buffer != NULL);
    /* Initialise buffer with nondeterministic data */
    for (size_t i = 0; i < current_size; ++i) {
        buffer[i] = nondet_uint8_t();
    }

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    /* Populate the list structure */
    list.alloc = alloc;
    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;
    list.data = buffer;

    /* Ensure the list satisfies the library’s validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Preserve original state for later checks */
    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data_ptr = list.data;

    /* --- Call the function under verification ------------------------------------------- */
    int ret = aws_array_list_pop_back(&list);

    /* --- Verify post‑conditions ---------------------------------------------------------- */
    /* Validity must still hold */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data_ptr);

    if (old_length > 0) {
        /* Successful pop */
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_length - 1);
        /* The bytes of the removed element are zeroed */
        size_t removed_offset = old_item_size * (old_length - 1);
        for (size_t i = 0; i < old_item_size; ++i) {
            assert(((uint8_t *)list.data)[removed_offset + i] == 0);
        }
    } else {
        /* List was empty */
        assert(ret != AWS_OP_SUCCESS);
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(list.length == 0);
    }

    /* Clean up */
    free(buffer);
    return 0;
}
