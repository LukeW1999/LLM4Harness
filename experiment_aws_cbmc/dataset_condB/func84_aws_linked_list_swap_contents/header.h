#ifndef AWS_COMMON_LINKED_LIST_H
#define AWS_COMMON_LINKED_LIST_H

#include <aws/common/common.h>

#include <stddef.h>

AWS_PUSH_SANE_WARNING_LEVEL

struct aws_linked_list_node {
    struct aws_linked_list_node *next;
    struct aws_linked_list_node *prev;
};

struct aws_linked_list {
    struct aws_linked_list_node head;
    struct aws_linked_list_node tail;
};

AWS_EXTERN_C_BEGIN

AWS_STATIC_IMPL void aws_linked_list_node_reset(struct aws_linked_list_node *node);

AWS_STATIC_IMPL bool aws_linked_list_empty(const struct aws_linked_list *list);

AWS_STATIC_IMPL bool aws_linked_list_is_valid(const struct aws_linked_list *list);

AWS_STATIC_IMPL bool aws_linked_list_node_next_is_valid(const struct aws_linked_list_node *node);

AWS_STATIC_IMPL bool aws_linked_list_node_prev_is_valid(const struct aws_linked_list_node *node);

AWS_STATIC_IMPL bool aws_linked_list_is_valid_deep(const struct aws_linked_list *list);

AWS_STATIC_IMPL void aws_linked_list_init(struct aws_linked_list *list);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_begin(const struct aws_linked_list *list);

AWS_STATIC_IMPL const struct aws_linked_list_node *aws_linked_list_end(const struct aws_linked_list *list);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_rbegin(const struct aws_linked_list *list);

AWS_STATIC_IMPL const struct aws_linked_list_node *aws_linked_list_rend(const struct aws_linked_list *list);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_next(const struct aws_linked_list_node *node);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_prev(const struct aws_linked_list_node *node);

AWS_STATIC_IMPL void aws_linked_list_insert_after(
    struct aws_linked_list_node *after,
    struct aws_linked_list_node *to_add);

AWS_STATIC_IMPL void aws_linked_list_swap_nodes(struct aws_linked_list_node *a, struct aws_linked_list_node *b);

AWS_STATIC_IMPL void aws_linked_list_insert_before(
    struct aws_linked_list_node *before,
    struct aws_linked_list_node *to_add);

AWS_STATIC_IMPL void aws_linked_list_remove(struct aws_linked_list_node *node);

AWS_STATIC_IMPL void aws_linked_list_push_back(struct aws_linked_list *list, struct aws_linked_list_node *node);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_back(const struct aws_linked_list *list);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_pop_back(struct aws_linked_list *list);

AWS_STATIC_IMPL void aws_linked_list_push_front(struct aws_linked_list *list, struct aws_linked_list_node *node);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_front(const struct aws_linked_list *list);

AWS_STATIC_IMPL struct aws_linked_list_node *aws_linked_list_pop_front(struct aws_linked_list *list);

AWS_STATIC_IMPL void aws_linked_list_swap_contents(
    struct aws_linked_list *AWS_RESTRICT a,
    struct aws_linked_list *AWS_RESTRICT b);

AWS_STATIC_IMPL void aws_linked_list_move_all_back(
    struct aws_linked_list *AWS_RESTRICT dst,
    struct aws_linked_list *AWS_RESTRICT src);

AWS_STATIC_IMPL void aws_linked_list_move_all_front(
    struct aws_linked_list *AWS_RESTRICT dst,
    struct aws_linked_list *AWS_RESTRICT src);

AWS_STATIC_IMPL bool aws_linked_list_node_is_in_list(struct aws_linked_list_node *node);
AWS_EXTERN_C_END

#ifndef AWS_NO_STATIC_IMPL
#    include <aws/common/linked_list.inl>
#endif 
AWS_POP_SANE_WARNING_LEVEL

#endif
