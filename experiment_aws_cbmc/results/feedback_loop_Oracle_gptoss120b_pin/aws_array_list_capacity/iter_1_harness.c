#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;
    /* nondet initialization of fields */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t current_size = nondet_size_t();
    size_t length = nondet_size_t();

    /* Ensure length * item_size does not overflow and fits within current_size */
    if (item_size != 0) {
        __CPROVER_assume(length <= current_size / item_size);
    }

    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;
    list.alloc = aws_default_allocator();

    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
        /* Fill data with nondet bytes */
        unsigned char *d = (unsigned char *)list.data;
        for (size_t i = 0; i < current_size; ++i) {
            d[i] = nondet_uchar();
        }
    } else {
        list.data = NULL;
    }

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the list structure and its data for frame condition checks */
    struct aws_array_list old_list = list;
    unsigned char *old_data = NULL;
    if (current_size > 0) {
        old_data = malloc(current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, current_size);
    }

    /* Call the function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* Postcondition 1: return value correctness */
    assert(capacity == current_size / item_size);

    /* Postcondition 2: length invariants (capacity does not affect length) */
    assert(list.length == old_list.length);

    /* Postcondition 3: frame conditions – list fields unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.alloc == old_list.alloc);
    assert(list.data == old_list.data);

    /* Frame condition – data buffer unchanged */
    if (current_size > 0) {
        assert(memcmp(list.data, old_data, current_size) == 0);
        free(old_data);
    }

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    if (list.data) {
        free(list.data);
    }

    return 0;
}
