#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

void aws_array_list_swap_contents_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size within bounds */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* nondet initial allocations within bounds */
    size_t init_a;
    __CPROVER_assume(init_a <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t init_b;
    __CPROVER_assume(init_b <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* initialize both lists dynamically */
    int r1 = aws_array_list_init_dynamic(&list_a, alloc, init_a, item_size);
    int r2 = aws_array_list_init_dynamic(&list_b, alloc, init_b, item_size);
    __CPROVER_assume(r1 == 0);
    __CPROVER_assume(r2 == 0);

    /* optionally push a nondet number of elements into each list */
    size_t push_a = nondet_size_t();
    __CPROVER_assume(push_a <= init_a);
    for (size_t i = 0; i < push_a; ++i) {
        uint8_t *buf = malloc(item_size);
        __CPROVER_assume(buf != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            buf[j] = nondet_uint8_t();
        }
        aws_array_list_push_back(&list_a, buf);
        free(buf);
    }

    size_t push_b = nondet_size_t();
    __CPROVER_assume(push_b <= init_b);
    for (size_t i = 0; i < push_b; ++i) {
        uint8_t *buf = malloc(item_size);
        __CPROVER_assume(buf != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            buf[j] = nondet_uint8_t();
        }
        aws_array_list_push_back(&list_b, buf);
        free(buf);
    }

    /* structural validity assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_b.alloc != NULL);
    __CPROVER_assume(list_a.item_size > 0);
    __CPROVER_assume(list_b.item_size > 0);
    __CPROVER_assume(list_a.item_size == list_b.item_size);

    /* save copies of the original structures */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* save copies of the underlying data buffers for frame condition */
    void *old_a_data = NULL;
    void *old_b_data = NULL;
    if (old_a.data != NULL) {
        old_a_data = malloc(old_a.current_size);
        __CPROVER_assume(old_a_data != NULL);
        memcpy(old_a_data, old_a.data, old_a.current_size);
    }
    if (old_b.data != NULL) {
        old_b_data = malloc(old_b.current_size);
        __CPROVER_assume(old_b_data != NULL);
        memcpy(old_b_data, old_b.data, old_b.current_size);
    }

    /* call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* postconditions */

    /* both lists remain valid */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* allocator unchanged and equal */
    assert(list_a.alloc == alloc);
    assert(list_b.alloc == alloc);
    assert(list_a.alloc == list_b.alloc);

    /* item size unchanged and equal */
    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);
    assert(list_a.item_size == list_b.item_size);

    /* length and current_size swapped */
    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);
    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    /* data pointers swapped */
    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* underlying data buffers unchanged */
    if (old_a_data != NULL && old_a.data != NULL) {
        assert(memcmp(old_a_data, old_a.data, old_a.current_size) == 0);
    }
    if (old_b_data != NULL && old_b.data != NULL) {
        assert(memcmp(old_b_data, old_b.data, old_b.current_size) == 0);
    }

    /* clean up auxiliary copies */
    free(old_a_data);
    free(old_b_data);

    return 0;
}
