#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic initialization parameters with bounds */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);
    __CPROVER_assume(initial_item_allocation <= 64);

    /* initialize list in dynamic mode */
    if (aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size) != 0) {
        return;
    }

    /* set a nondeterministic valid length with bounds */
    size_t capacity = aws_array_list_capacity(&list);
    __CPROVER_assume(capacity <= 64);
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    __CPROVER_assume(length <= 64);
    list.length = length;

    /* optionally fill the list's data with nondeterministic bytes */
    if (list.data != NULL && list.length > 0) {
        uint8_t *bytes = (uint8_t *)list.data;
        size_t total_bytes = list.length * list.item_size;
        __CPROVER_assume(total_bytes <= 4096);
        for (size_t i = 0; i < total_bytes; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* nondeterministic number of elements to pop with bound */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= 64);

    /* pre‑condition: the list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of relevant state before the call */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    uint8_t *old_contents = NULL;
    if (list.data != NULL && list.length > 0) {
        size_t old_bytes = list.length * list.item_size;
        __CPROVER_assume(old_bytes <= 4096);
        old_contents = malloc(old_bytes);
        __CPROVER_assume(old_contents != NULL);
        for (size_t i = 0; i < old_bytes; ++i) {
            old_contents[i] = ((uint8_t *)list.data)[i];
        }
    }

    /* call the function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* post‑conditions */
    size_t expected_length = (n <= old_length) ? (old_length - n) : old_length;
    assert(list.length == expected_length);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(aws_array_list_is_valid(&list));

    if (n <= old_length && list.data != NULL && list.length > 0) {
        uint8_t *new_bytes = (uint8_t *)list.data;
        size_t new_total = list.length * list.item_size;
        for (size_t i = 0; i < new_total; ++i) {
            assert(new_bytes[i] == old_contents[i + n * old_item_size]);
        }
    }

    /* clean up */
    aws_array_list_clean_up(&list);
    free(old_contents);
}
