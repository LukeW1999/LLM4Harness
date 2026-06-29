#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    struct aws_allocator *allocator = aws_default_allocator();

    /* Use a small bounded item_size to avoid huge mallocs */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);

    /* Set up list_a */
    list_a.alloc = allocator;
    list_a.item_size = item_size;

    size_t current_size_a;
    size_t length_a;
    __CPROVER_assume(current_size_a <= 64);
    __CPROVER_assume(current_size_a % item_size == 0);
    __CPROVER_assume(length_a <= current_size_a / item_size);

    list_a.current_size = current_size_a;
    list_a.length = length_a;

    if (current_size_a > 0) {
        void *data_a = malloc(current_size_a);
        __CPROVER_assume(data_a != NULL);
        list_a.data = data_a;
    } else {
        list_a.data = NULL;
    }

    /* Set up list_b */
    list_b.alloc = allocator;
    list_b.item_size = item_size;

    size_t current_size_b;
    size_t length_b;
    __CPROVER_assume(current_size_b <= 64);
    __CPROVER_assume(current_size_b % item_size == 0);
    __CPROVER_assume(length_b <= current_size_b / item_size);

    list_b.current_size = current_size_b;
    list_b.length = length_b;

    if (current_size_b > 0) {
        void *data_b = malloc(current_size_b);
        __CPROVER_assume(data_b != NULL);
        list_b.data = data_b;
    } else {
        list_b.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Save old values */
    void *old_data_a = list_a.data;
    void *old_data_b = list_b.data;
    size_t old_length_a = list_a.length;
    size_t old_length_b = list_b.length;
    size_t old_current_size_a = list_a.current_size;
    size_t old_current_size_b = list_b.current_size;

    /* Call the function under verification */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* Postconditions */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    assert(list_a.data == old_data_b);
    assert(list_b.data == old_data_a);

    assert(list_a.length == old_length_b);
    assert(list_b.length == old_length_a);

    assert(list_a.current_size == old_current_size_b);
    assert(list_b.current_size == old_current_size_a);

    assert(list_a.item_size == item_size);
    assert(list_b.item_size == item_size);

    assert(list_a.alloc == allocator);
    assert(list_b.alloc == allocator);
}
