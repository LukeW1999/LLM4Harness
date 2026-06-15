/* CBMC harness for aws_array_list_swap */

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

#define MAX_ITEM_SIZE   32
#define MAX_CAPACITY    8
#define MAX_LENGTH      8

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* nondet capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_CAPACITY);

    /* initialize list */
    int init_ret = aws_array_list_init_dynamic(&list, alloc, capacity, item_size);
    __CPROVER_assume(init_ret == 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet length (must be >0 for swap) */
    size_t len = nondet_size_t();
    __CPROVER_assume(len > 0 && len <= capacity && len <= MAX_LENGTH);

    /* populate list with nondet elements */
    for (size_t i = 0; i < len; ++i) {
        uint8_t *elem = malloc(item_size);
        __CPROVER_assume(elem != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = __CPROVER_nondet_uint8_t();
        }
        int push_ret = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_ret == 0);
        free(elem);
    }

    __CPROVER_assume(list.length == len);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* choose indices a and b */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    /* save copies of elements before swap */
    uint8_t *pre_a = malloc(item_size);
    uint8_t *pre_b = malloc(item_size);
    __CPROVER_assume(pre_a && pre_b);
    int get_ret_a = aws_array_list_get_at(&list, pre_a, a);
    int get_ret_b = aws_array_list_get_at(&list, pre_b, b);
    __CPROVER_assume(get_ret_a == 0 && get_ret_b == 0);

    uint8_t *pre_others[MAX_LENGTH];
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            pre_others[i] = malloc(item_size);
            __CPROVER_assume(pre_others[i] != NULL);
            int ret = aws_array_list_get_at(&list, pre_others[i], i);
            __CPROVER_assume(ret == 0);
        } else {
            pre_others[i] = NULL;
        }
    }

    /* perform swap */
    aws_array_list_swap(&list, a, b);

    /* postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.length == len);
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);

    /* verify swapped elements */
    uint8_t *post_a = malloc(item_size);
    uint8_t *post_b = malloc(item_size);
    __CPROVER_assume(post_a && post_b);
    int post_ret_a = aws_array_list_get_at(&list, post_a, a);
    int post_ret_b = aws_array_list_get_at(&list, post_b, b);
    __CPROVER_assume(post_ret_a == 0 && post_ret_b == 0);
    assert(__CPROVER_memcmp(post_a, pre_b, item_size) == 0);
    assert(__CPROVER_memcmp(post_b, pre_a, item_size) == 0);
    free(post_a);
    free(post_b);

    /* verify other elements unchanged */
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            uint8_t *post_i = malloc(item_size);
            __CPROVER_assume(post_i != NULL);
            int ret = aws_array_list_get_at(&list, post_i, i);
            __CPROVER_assume(ret == 0);
            assert(__CPROVER_memcmp(post_i, pre_others[i], item_size) == 0);
            free(post_i);
        }
    }

    /* clean up */
    aws_array_list_clean_up(&list);
    free(pre_a);
    free(pre_b);
    for (size_t i = 0; i < len; ++i) {
        if (pre_others[i] != NULL) {
            free(pre_others[i]);
        }
    }
}
