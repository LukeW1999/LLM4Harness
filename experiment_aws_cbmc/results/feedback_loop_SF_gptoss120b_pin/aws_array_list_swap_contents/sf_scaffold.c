#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

size_t nondet_size_t(void);

void aws_array_list_swap_contents_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet inputs */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity_a = nondet_size_t();
    size_t capacity_b = nondet_size_t();
    __CPROVER_assume(capacity_a <= 10);
    __CPROVER_assume(capacity_b <= 10);

    size_t length_a = nondet_size_t();
    size_t length_b = nondet_size_t();
    __CPROVER_assume(length_a <= capacity_a);
    __CPROVER_assume(length_b <= capacity_b);

    void *data_a = malloc(item_size * capacity_a);
    void *data_b = malloc(item_size * capacity_b);
    __CPROVER_assume((capacity_a == 0) || data_a);
    __CPROVER_assume((capacity_b == 0) || data_b);

    struct aws_array_list list_a;
    list_a.alloc = alloc;
    list_a.item_size = item_size;
    list_a.length = length_a;
    list_a.current_size = capacity_a * item_size;
    list_a.data = data_a;

    struct aws_array_list list_b;
    list_b.alloc = alloc;
    list_b.item_size = item_size;
    list_b.length = length_b;
    list_b.current_size = capacity_b * item_size;
    list_b.data = data_b;

    /* preconditions */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    /* snapshot pre‑call state */
    struct aws_allocator *alloc_a_snapshot = list_a.alloc;
    struct aws_allocator *alloc_b_snapshot = list_b.alloc;
    size_t item_size_snapshot = list_a.item_size;
    size_t length_a_snapshot = list_a.length;
    size_t length_b_snapshot = list_b.length;
    size_t current_size_a_snapshot = list_a.current_size;
    size_t current_size_b_snapshot = list_b.current_size;
    void *data_a_snapshot = list_a.data;
    void *data_b_snapshot = list_b.data;

    /* call under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* ASSERT_POSTCONDITIONS_HERE */

    /* clean up */
    free(data_a);
    free(data_b);
}
