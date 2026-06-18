/*=== Contract ===
Preconditions:
  - __CPROVER_assume(list != NULL);
  - __CPROVER_assume(raw_array != NULL);
  - __CPROVER_assume(item_count > 0);
  - __CPROVER_assume(item_size > 0);
  - __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &tmp_current_size));
Postconditions (validity):
  - assert(list->alloc == NULL);
  - assert(list->item_size == item_size);
  - assert(list->length == 0);
  - assert(list->data == raw_array);
  - assert(list->current_size == tmp_current_size);
  - assert(aws_array_list_is_valid(list));
Postconditions (frame):
  - No memory outside of &list and raw_array is modified.
===*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    size_t tmp_current_size;
    /* Ensure multiplication does not overflow */
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &tmp_current_size));

    void *raw_array = malloc(tmp_current_size);
    __CPROVER_assume(raw_array != NULL);

    /* Save copies of memory that should remain unchanged */
    struct aws_array_list list_before = *list;
    unsigned char *raw_before = malloc(tmp_current_size);
    __CPROVER_assume(raw_before != NULL);
    memcpy(raw_before, raw_array, tmp_current_size);

    aws_array_list_init_static(list, raw_array, item_count, item_size);

    /* Postconditions */
    assert(list->alloc == NULL);
    assert(list->item_size == item_size);
    assert(list->length == 0);
    assert(list->data == raw_array);
    assert(list->current_size == tmp_current_size);
    assert(aws_array_list_is_valid(list));

    /* Frame conditions */
    assert(memcmp(raw_before, raw_array, tmp_current_size) == 0);
    assert(memcmp(&list_before, list, sizeof(struct aws_array_list)) == 0 ||
           (list_before.alloc == list->alloc && /* alloc is NULL before and after */
            list_before.item_size == list->item_size && /* unchanged except set to item_size */
            list_before.length == list->length &&       /* length set to 0 */
            list_before.data == list->data &&           /* data unchanged */
            list_before.current_size == list->current_size)); /* current_size set */

    free(raw_before);
    free(raw_array);
    free(list);
    return 0;
}
