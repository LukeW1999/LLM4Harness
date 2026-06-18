#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* ---------- s2n data structures (as given) ---------- */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* ---------- external functions used by the implementation ---------- */
int s2n_alloc(struct s2n_blob *blob, uint32_t size);
int s2n_stuffer_init(struct s2n_stuffer *stuffer, struct s2n_blob *blob);
bool s2n_stuffer_validate(const struct s2n_stuffer *stuffer);

/* ---------- macros used in the implementation ---------- */
#define S2N_SUCCESS 0
#define S2N_ERR    -1

#define POSIX_ENSURE_REF(ptr)               \
    do { if ((ptr) == NULL) return S2N_ERR; } while (0)

#define POSIX_GUARD(expr)                                   \
    do { int __rc = (expr); if (__rc != S2N_SUCCESS) return __rc; } while (0)

#define POSIX_POSTCONDITION(expr) assert(expr)

/* ---------- function under test (as provided) ---------- */
int s2n_stuffer_alloc(struct s2n_stuffer *stuffer, const uint32_t size)
{
    POSIX_ENSURE_REF(stuffer);
    *stuffer = (struct s2n_stuffer){ 0 };
    POSIX_GUARD(s2n_alloc(&stuffer->blob, size));
    POSIX_GUARD(s2n_stuffer_init(stuffer, &stuffer->blob));

    stuffer->alloced = 1;

    POSIX_POSTCONDITION(s2n_stuffer_validate(stuffer));
    return S2N_SUCCESS;
}

/* ---------- nondeterministic helpers (from proof_helpers) ---------- */
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);
uint32_t nondet_uint32_t(void);
uint64_t nondet_uint64_t(void);
size_t nondet_size_t(void);

/* ---------- harness ---------- */
void s2n_stuffer_alloc_harness(void) {
    /* 1. Declare the stuffer and a nondet size */
    struct s2n_stuffer stuffer;
    uint32_t size = nondet_uint32_t();

    /* bound the size to keep the state space finite */
    __CPROVER_assume(size <= 1024U);

    /* 2. Save the old state (before the call) */
    struct s2n_stuffer old = stuffer;

    /* 3. Call the function under test */
    int result = s2n_stuffer_alloc(&stuffer, size);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == S2N_SUCCESS) {
        /* fields that must be set on success */
        assert(stuffer.alloced == 1);
        assert(stuffer.blob.size == size);
        assert(stuffer.blob.allocated >= size);
        assert(stuffer.blob.data != NULL);
        assert(stuffer.blob.growable == 1);
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
        /* the stuffer must validate */
        assert(s2n_stuffer_validate(&stuffer));
    } else {
        /* on failure the struct should remain zeroed (as it was after the
           initial memset) */
        assert(stuffer.alloced == 0);
        assert(stuffer.blob.data == NULL);
        assert(stuffer.blob.size == 0);
        assert(stuffer.blob.allocated == 0);
        assert(stuffer.blob.growable == 0);
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
        /* unchanged fields relative to the zeroed state */
        assert(stuffer.growable == 0);
        assert(stuffer.tainted == 0);
    }

    /* 5. The result must be one of the defined return codes */
    assert(result == S2N_SUCCESS || result == S2N_ERR);
}
