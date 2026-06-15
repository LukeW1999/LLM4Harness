#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size (must be > 0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024); /* bound for CBMC */

    /* allocate two distinct list structures */
    struct aws_array_list *list_a = malloc(sizeof(struct aws_array_list));
    struct aws_array_list *list_b = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list_a != NULL);
    __CPROVER_assume(list_b != NULL);
    __CPROVER_assume(list_a != list_b);

    /* nondet capacities for dynamic allocation (must be > 0) */
    size_t cap_a, cap_b;
    __CPROVER_assume(cap_a > 0);
    __CPROVER_assume(cap_b > 0);
    __CPROVER_assume(cap_a < 256);
    __CPROVER_assume(cap_b < 256);

    /* initialise both lists in dynamic mode with the same allocator and item size */
    int init_res_a = aws_array_list_init_dynamic(list_a, alloc, cap_a, item_size);
    int init_res_b = aws_array_list_init_dynamic(list_b, alloc, cap_b, item_size);
    __CPROVER_assume(init_res_a == 0);
    __CPROVER_assume(init_res_b == 0);

    /* nondet lengths that respect the capacities */
    size_t len_a, len_b;
    __CPROVER_assume(len_a <= cap_a);
    __CPROVER_assume(len_b <= cap_b);
    list_a->length = len_a;
    list_a->current_size = cap_a * item_size;
    list_b->length = len_b;
    list_b->current_size = cap_b * item_size;

    /* fill the underlying buffers with nondet data */
    uint8_t *buf_a = (uint8_t *)list_a->data;
    uint8_t *buf_b = (uint8_t *)list_b->data;
    for (size_t i = 0; i < cap_a * item_size; ++i) {
        buf_a[i] = nondet_uint8_t();
    }
    for (size_t i = 0; i < cap_b * item_size; ++i) {
        buf_b[i] = nondet_uint8_t();
    }

    /* capture pre‑state values */
    void *data_a_orig = list_a->data;
    void *data_b_orig = list_b->data;
    size_t len_a_orig = list_a->length;
    size_t len_b_orig = list_b->length;
    size_t cur_a_orig = list_a->current_size;
    size_t cur_b_orig = list_b->current_size;

    /* make copies of the raw buffers to check they are unchanged */
    uint8_t *copy_a = malloc(cap_a * item_size);
    uint8_t *copy_b = malloc(cap_b * item_size);
    __CPROVER_assume(copy_a != NULL);
    __CPROVER_assume(copy_b != NULL);
    memcpy(copy_a, buf_a, cap_a * item_size);
    memcpy(copy_b, buf_b, cap_b * item_size);

    /* Preconditions: both lists are valid before the call */
    assert(aws_array_list_is_valid(list_a));
    assert(aws_array_list_is_valid(list_b));

    /* call the function under test */
    aws_array_list_swap_contents(list_a, list_b);

    /* Postconditions: both lists remain valid */
    assert(aws_array_list_is_valid(list_a));
    assert(aws_array_list_is_valid(list_b));

    /* lengths and capacities are swapped */
    assert(list_a->length == len_b_orig);
    assert(list_b->length == len_a_orig);
    assert(list_a->current_size == cur_b_orig);
    assert(list_b->current_size == cur_a_orig);

    /* data pointers are exchanged */
    assert(list_a->data == data_b_orig);
    assert(list_b->data == data_a_orig);

    /* underlying buffers are unchanged */
    assert(memcmp(copy_a, (uint8_t *)list_b->data, cap_a * item_size) == 0);
    assert(memcmp(copy_b, (uint8_t *)list_a->data, cap_b * item_size) == 0);

    /* clean up */
    aws_array_list_clean_up(list_a);
    aws_array_list_clean_up(list_b);
    free(list_a);
    free(list_b);
    free(copy_a);
    free(copy_b);
}
