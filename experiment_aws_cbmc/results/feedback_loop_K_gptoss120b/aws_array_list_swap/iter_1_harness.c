/*  
 * Contract for aws_array_list_swap  
 * Preconditions:  
 *   - list is a valid, initialized aws_array_list (aws_array_list_is_valid(list) == true)  
 *   - list->length > 0  
 *   - a and b are indices such that 0 <= a < list->length and 0 <= b < list->length  
 *   - list->item_size > 0  
 * Postconditions (validity):  
 *   - aws_array_list_is_valid(list) remains true after the call  
 *   - list->alloc is unchanged  
 *   - list->item_size is unchanged  
 *   - list->length is unchanged  
 *   - list->capacity (implicit via current_size) is unchanged  
 * Postconditions (element swap):  
 *   - The element previously at index a is now at index b  
 *   - The element previously at index b is now at index a  
 *   - All other elements (indices != a && != b) are unchanged  
 * Postconditions (frame):  
 *   - No memory outside the list's internal buffer is modified  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEM_SIZE 32
#define MAX_INITIAL_ALLOC 8
#define MAX_LENGTH 8

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* nondet initial allocation (capacity) */
    size_t initial_alloc;
    __CPROVER_assume(initial_alloc > 0);
    __CPROVER_assume(initial_alloc <= MAX_INITIAL_ALLOC);

    /* initialize dynamic list */
    int init_ret = aws_array_list_init_dynamic(&list, alloc, initial_alloc, item_size);
    __CPROVER_assume(init_ret == 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet length (number of elements to populate) */
    size_t len;
    __CPROVER_assume(len <= initial_alloc);
    __CPROVER_assume(len <= MAX_LENGTH);

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
    size_t a, b;
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    /* save copies of all elements before swap */
    uint8_t *pre_a = malloc(item_size);
    uint8_t *pre_b = malloc(item_size);
    uint8_t *pre_others[MAX_LENGTH];
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            pre_others[i] = malloc(item_size);
        } else {
            pre_others[i] = NULL;
        }
    }

    int get_ret_a = aws_array_list_get_at(&list, pre_a, a);
    __CPROVER_assume(get_ret_a == 0);
    int get_ret_b = aws_array_list_get_at(&list, pre_b, b);
    __CPROVER_assume(get_ret_b == 0);
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            int get_ret = aws_array_list_get_at(&list, pre_others[i], i);
            __CPROVER_assume(get_ret == 0);
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
    int post_ret_a = aws_array_list_get_at(&list, post_a, a);
    int post_ret_b = aws_array_list_get_at(&list, post_b, b);
    __CPROVER_assume(post_ret_a == 0);
    __CPROVER_assume(post_ret_b == 0);
    assert(__CPROVER_memcmp(post_a, pre_b, item_size) == 0);
    assert(__CPROVER_memcmp(post_b, pre_a, item_size) == 0);
    free(post_a);
    free(post_b);

    /* verify other elements unchanged */
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            uint8_t *post_i = malloc(item_size);
            int post_ret = aws_array_list_get_at(&list, post_i, i);
            __CPROVER_assume(post_ret == 0);
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

    return 0;
}
