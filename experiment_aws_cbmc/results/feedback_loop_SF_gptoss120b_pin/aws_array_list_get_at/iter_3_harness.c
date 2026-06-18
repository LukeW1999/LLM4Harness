#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

size_t nondet_size_t(void);
void *nondet_void_ptr(void);

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* allocator */
    list.alloc = allocator;

    /* nondet item size, must be > 0 and bounded */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 32);

    /* nondet current size, bounded */
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= 128);

    /* nondet length, bounded */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= 8);

    /* ensure length * item_size does not overflow and fits in current_size */
    size_t required_bytes;
    __CPROVER_assume(!aws_mul_size_checked(list.length, list.item_size, &required_bytes));
    __CPROVER_assume(required_bytes <= list.current_size);

    /* allocate data buffer if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* nondet index, bounded to keep exploration reasonable */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= list.length + 4);

    /* allocate output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* preconditions */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val != NULL);

    /* snapshot of input state */
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    uint8_t *old_contents = NULL;
    if (list.current_size > 0) {
        old_contents = malloc(list.current_size);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, list.data, list.current_size);
    }

    /* call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* successful get requires a valid index */
        assert(index < old_length);
        /* the returned value must match the element at the given index */
        if (old_contents != NULL) {
            for (size_t i = 0; i < list.item_size; ++i) {
                assert(((uint8_t *)val)[i] == old_contents[index * list.item_size + i]);
            }
        }
    } else {
        /* error case occurs when index is out of bounds */
        assert(index >= old_length);
        /* val must remain unchanged on error (conservatively assumed) */
    }

    /* frame conditions: the list structure must remain unchanged */
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size > 0);
    assert(list.alloc == allocator);
    assert(list.data == old_data);

    if (old_contents != NULL) {
        for (size_t i = 0; i < old_current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_contents[i]);
        }
    }

    /* cleanup */
    free(old_contents);
    free(val);
    free(list.data);
}
