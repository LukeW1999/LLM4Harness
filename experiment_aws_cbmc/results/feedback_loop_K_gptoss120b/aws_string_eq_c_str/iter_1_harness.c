/*  
 * Contract for aws_string_eq_c_str  
 * --------------------------------  
 * Preconditions:  
 *   - If str != NULL, then aws_string_is_valid(str) holds.  
 *   - If c_str != NULL, then aws_c_string_is_valid(c_str) holds.  
 *   - The allocator used for any dynamic allocation is aws_default_allocator().  
 *   - The memory regions pointed to by str, str->bytes, and c_str are allocated and accessible.  
 *   - No aliasing: str and c_str point to distinct memory regions (they may be NULL).  
 *   - The length field str->len is a size_t value consistent with the allocated bytes.  
 *   - The C‑string c_str, if non‑NULL, is null‑terminated within a reasonable bound (e.g., MAX_CSTR_LEN).  
 *   - All allocated buffers are initialized with nondeterministic data.  
 *  
 * Postconditions (validity):  
 *   - The function returns true iff one of the following holds:  
 *       * str == NULL && c_str == NULL  
 *       * str != NULL && c_str != NULL && str->len == strlen(c_str) && the first str->len bytes of str->bytes equal the bytes of c_str.  
 *   - The function never returns an error code; it returns a bool.  
 *  
 * Postconditions (frame):  
 *   - The contents of str, str->bytes, str->len, and c_str are unchanged.  
 *   - No other memory locations are modified.  
 */  

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STR_LEN 64
#define MAX_CSTR_LEN 64

/* Helper to compute strlen in CBMC (nondet bounded) */
static size_t cbmc_strlen(const char *s) {
    size_t i = 0;
    while (i < MAX_CSTR_LEN && s[i] != '\0') {
        i++;
    }
    __CPROVER_assume(i < MAX_CSTR_LEN); /* ensure null‑termination within bound */
    return i;
}

/* Helper to compare array bytes with a C‑string */
static bool expected_eq(const uint8_t *bytes, size_t len, const char *c_str) {
    size_t c_len = cbmc_strlen(c_str);
    if (len != c_len) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        if (bytes[i] != (uint8_t)c_str[i]) {
            return false;
        }
    }
    return true;
}

/* Snapshot function for frame condition */
static void snapshot_memory(const void *src, void *dst, size_t n) {
    const uint8_t *s = (const uint8_t *)src;
    uint8_t *d = (uint8_t *)dst;
    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }
}

/* Allocate a nondeterministic aws_string (or NULL) */
static struct aws_string *make_nondet_string(void) {
    struct aws_string *s = NULL;
    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_STR_LEN);
        /* allocate space for struct + flexible array */
        size_t total = sizeof(struct aws_string) + (len - 1) * sizeof(uint8_t);
        s = (struct aws_string *)malloc(total);
        __CPROVER_assume(s != NULL);
        s->allocator = aws_default_allocator();
        s->len = len;
        for (size_t i = 0; i < len; ++i) {
            s->bytes[i] = __CPROVER_nondet_uint8_t();
        }
        /* ensure string validity */
        __CPROVER_assume(aws_string_is_valid(s));
    }
    return s;
}

/* Allocate a nondeterministic C‑string (or NULL) */
static char *make_nondet_c_str(void) {
    char *c = NULL;
    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len < MAX_CSTR_LEN); /* leave room for null */
        c = (char *)malloc(len + 1);
        __CPROVER_assume(c != NULL);
        for (size_t i = 0; i < len; ++i) {
            c[i] = (char)__CPROVER_nondet_uint8_t();
        }
        c[len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c));
    }
    return c;
}

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str = make_nondet_string();
    char *c_str = make_nondet_c_str();

    /* Frame snapshots */
    struct aws_string str_snapshot;
    if (str != NULL) {
        str_snapshot.allocator = str->allocator;
        str_snapshot.len = str->len;
        uint8_t *bytes_snapshot = (uint8_t *)malloc(str->len);
        __CPROVER_assume(bytes_snapshot != NULL);
        snapshot_memory(str->bytes, bytes_snapshot, str->len);
        /* Snapshot the whole struct (excluding flexible array) */
        struct aws_string *str_copy = (struct aws_string *)malloc(sizeof(struct aws_string));
        __CPROVER_assume(str_copy != NULL);
        str_copy->allocator = str->allocator;
        str_copy->len = str->len;
        /* Save pointer to original bytes for later compare */
        str_snapshot.bytes[0] = 0; /* unused, just to keep compiler happy */
        /* We'll compare bytes via the separate snapshot buffer */
        (void)str_copy; /* silence unused warning */
        (void)bytes_snapshot;
    }

    char *c_str_snapshot = NULL;
    size_t c_str_len = 0;
    if (c_str != NULL) {
        c_str_len = cbmc_strlen(c_str);
        c_str_snapshot = (char *)malloc(c_str_len + 1);
        __CPROVER_assume(c_str_snapshot != NULL);
        for (size_t i = 0; i <= c_str_len; ++i) {
            c_str_snapshot[i] = c_str[i];
        }
    }

    bool result = aws_string_eq_c_str(str, c_str);

    /* Postcondition: result matches the specification */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = expected_eq(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* Frame condition: str unchanged */
    if (str != NULL) {
        assert(str->allocator == str_snapshot.allocator);
        assert(str->len == str_snapshot.len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(str->bytes[i] == ((uint8_t *)bytes_snapshot)[i]);
        }
    }

    /* Frame condition: c_str unchanged */
    if (c_str != NULL) {
        for (size_t i = 0; i <= c_str_len; ++i) {
            assert(c_str[i] == c_str_snapshot[i]);
        }
    }

    return 0;
}
