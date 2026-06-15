/*=== Contract ===
Preconditions:
  - __CPROVER_assume(str == NULL || aws_string_is_valid(str));
  - __CPROVER_assume(cur == NULL || (cur->ptr == NULL ? cur->len == 0 : cur->len >= 0));
  - __CPROVER_assume(str == NULL || str->allocator == aws_default_allocator() || str->allocator == NULL);
Postconditions (validity):
  - The function does not dereference NULL pointers.
  - The return value is true iff (str == NULL && cur == NULL) ||
    (str != NULL && cur != NULL && str->len == cur->len && 
     memcmp(str->bytes, cur->ptr, str->len) == 0).
Postconditions (frame):
  - No memory reachable from str or cur is modified by the function.
=== End Contract ===*/

#include <proof_helpers/make_common_data_structures.h>

/* Helper to nondeterministically allocate a string */
static struct aws_string *nondet_aws_string(void) {
    struct aws_string *s;
    __CPROVER_assume(s == NULL || __CPROVER_is_fresh(s, sizeof(*s)));
    if (s != NULL) {
        size_t len;
        __CPROVER_assume(len <= 256);
        s = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(s != NULL);
        s->allocator = aws_default_allocator();
        s->len = len;
        if (len > 0) {
            __CPROVER_assume(__CPROVER_is_fresh(s->bytes, len));
        }
    }
    return s;
}

/* Helper to nondeterministically allocate a byte cursor */
static struct aws_byte_cursor *nondet_aws_byte_cursor(void) {
    struct aws_byte_cursor *c;
    __CPROVER_assume(c == NULL || __CPROVER_is_fresh(c, sizeof(*c)));
    if (c != NULL) {
        size_t len;
        __CPROVER_assume(len <= 256);
        c->len = len;
        if (len > 0) {
            c->ptr = (uint8_t *)malloc(len);
            __CPROVER_assume(c->ptr != NULL);
            __CPROVER_assume(__CPROVER_is_fresh(c->ptr, len));
        } else {
            c->ptr = NULL;
        }
    }
    return c;
}

/* Snapshot helpers */
static void snapshot_string(const struct aws_string *src, struct aws_string *dst) {
    if (src == NULL) return;
    memcpy((void *)dst, src, sizeof(struct aws_string) + src->len);
}
static void snapshot_cursor(const struct aws_byte_cursor *src, struct aws_byte_cursor *dst) {
    if (src == NULL) return;
    dst->len = src->len;
    if (src->ptr != NULL && src->len > 0) {
        dst->ptr = (uint8_t *)malloc(src->len);
        __CPROVER_assume(dst->ptr != NULL);
        memcpy(dst->ptr, src->ptr, src->len);
    } else {
        dst->ptr = NULL;
    }
}

/* Equality check used for postcondition */
static bool eq_bytes(const struct aws_string *s, const struct aws_byte_cursor *c) {
    if (s->len != c->len) return false;
    return (memcmp(s->bytes, c->ptr, s->len) == 0);
}

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str = nondet_aws_string();
    struct aws_byte_cursor *cur = nondet_aws_byte_cursor();

    /* Apply preconditions */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));
    __CPROVER_assume(cur == NULL || (cur->ptr == NULL ? cur->len == 0 : cur->len >= 0));

    /* Take snapshots for frame condition */
    struct aws_string *str_snapshot = NULL;
    struct aws_byte_cursor *cur_snapshot = NULL;
    if (str != NULL) {
        str_snapshot = (struct aws_string *)malloc(sizeof(struct aws_string) + str->len);
        __CPROVER_assume(str_snapshot != NULL);
        snapshot_string(str, str_snapshot);
    }
    if (cur != NULL) {
        cur_snapshot = (struct aws_byte_cursor *)malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur_snapshot != NULL);
        snapshot_cursor(cur, cur_snapshot);
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postcondition: return value correctness */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        assert(result == eq_bytes(str, cur));
    }

    /* Frame condition: no modifications to inputs */
    if (str != NULL) {
        assert(memcmp(str, str_snapshot, sizeof(struct aws_string) + str->len) == 0);
    }
    if (cur != NULL) {
        assert(cur->len == cur_snapshot->len);
        if (cur->ptr != NULL && cur_snapshot->ptr != NULL) {
            assert(memcmp(cur->ptr, cur_snapshot->ptr, cur->len) == 0);
        } else {
            assert(cur->ptr == cur_snapshot->ptr);
        }
    }
}
