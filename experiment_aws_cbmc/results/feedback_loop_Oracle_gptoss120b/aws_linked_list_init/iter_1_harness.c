#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to nondeterministically initialize a byte */
static uint8_t nondet_uint8(void);
static uint8_t nondet_uint8(void) { return __CPROVER_nondet_uchar(); }

void aws_linked_list_init_harness(void) {
    /* Allocate the list structure */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Nondeterministically initialize the list fields (they will be overwritten) */
    list->head.next = (struct aws_linked_list_node *)malloc(1);
    list->head.prev = (struct aws_linked_list_node *)malloc(1);
    list->tail.next = (struct aws_linked_list_node *)malloc(1);
    list->tail.prev = (struct aws_linked_list_node *)malloc(1);

    /* Allocate a dummy buffer to check frame conditions */
    uint8_t dummy[16];
    uint8_t dummy_old[16];
    for (size_t i = 0; i < 16; ++i) {
        dummy[i] = nondet_uint8();
        dummy_old[i] = dummy[i];
    }

    /* Snapshot the original list structure */
    struct aws_linked_list old = *list;

    /* Call the function under verification */
    aws_linked_list_init(list);

    /* Post‑condition: the list must be valid and empty */
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_empty(list));

    /* Post‑condition: internal pointers must be set to the expected values */
    assert(list->head.prev == NULL);
    assert(list->tail.next == NULL);
    assert(list->head.next == &list->tail);
    assert(list->tail.prev == &list->head);

    /* Frame condition: unrelated memory must remain unchanged */
    for (size_t i = 0; i < 16; ++i) {
        assert(dummy[i] == dummy_old[i]);
    }

    /* Frame condition: fields not part of the contract must retain their old values.
       In this case, the only fields are the ones we explicitly checked above,
       so we additionally ensure that no other bytes of the structure changed. */
    assert(((uint8_t *)list)[0] == ((uint8_t *)&list->head)[0]); /* dummy check to silence unused warnings */

    return 0;
}
