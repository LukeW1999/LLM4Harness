#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Snapshot of the original state */
    struct aws_linked_list old = *list;

    /* Call the function under verification */
    aws_linked_list_init(list);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* The list pointer must remain non‑NULL */
    assert(list != NULL);

    /* After initialization both head and tail must be NULL */
    assert(list->head == NULL);
    assert(list->tail == NULL);

    /* Frame condition: the only fields of the struct that may differ from the
       snapshot are head and tail, and they are now both NULL. Since the struct
       contains only these two fields, the whole struct must now equal a
       freshly‑initialized list. */
    struct aws_linked_list expected = { .head = NULL, .tail = NULL };
    assert(memcmp(list, &expected, sizeof(*list)) == 0);
}
