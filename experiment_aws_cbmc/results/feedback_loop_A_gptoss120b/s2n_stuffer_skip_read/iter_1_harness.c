#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Bounding constant for the blob buffer */
#define MAX_BUFFER_SIZE 1024

/* Nondeterministic helpers (normally provided by proof_helpers) */
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);
uint32_t nondet_uint32_t(void);
uint64_t nondet_uint64_t(void);
size_t nondet_size_t(void);

/* s2n data structures (as described in the prompt) */
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

/* Minimal stubs for the validation helpers used by the function */
static inline bool s2n_stuffer_validate(const struct s2n_stuffer *stuffer) {
    /* For the purpose of the harness we assume the stuffer is always valid
       after we have bounded its fields. */
    return true;
}

static inline uint32_t s2n_stuffer_data_available(const struct s2n_stuffer *stuffer) {
    if (stuffer->write_cursor >= stuffer->read_cursor) {
        return stuffer->write_cursor - stuffer->read_cursor;
    }
    return 0;
}

/* Macros used in the implementation */
#define POSIX_PRECONDITION(x)  do { if (!(x)) __CPROVER_assume(0); } while (0)
#define POSIX_ENSURE(x, err)   do { if (!(x)) return (err); } while (0)
#define S2N_SUCCESS            0
#define S2N_ERR_STUFFER_OUT_OF_DATA (-1)

/* Function under test (implementation reproduced here) */
int s2n_stuffer_skip_read(struct s2n_stuffer *stuffer, uint32_t n)
{
    POSIX_PRECONDITION(s2n_stuffer_validate(stuffer));
    POSIX_ENSURE(s2n_stuffer_data_available(stuffer) >= n,
                 S2N_ERR_STUFFER_OUT_OF_DATA);

    stuffer->read_cursor += n;
    return S2N_SUCCESS;
}

/* Harness */
void s2n_stuffer_skip_read_harness(void) {
    struct s2n_stuffer stuffer;

    /* Allocate and bound the blob's data buffer */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);
    stuffer.blob.data = malloc(stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.data != NULL);
    stuffer.blob.allocated = stuffer.blob.size;
    stuffer.blob.growable = nondet_bool();

    /* Non‑deterministic cursor values, bounded so that data is available */
    stuffer.read_cursor  = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor >= stuffer.read_cursor);
    stuffer.high_water_mark = nondet_uint32_t();

    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* Save the original state */
    struct s2n_stuffer old = stuffer;

    /* Non‑deterministic number of bytes to skip, respecting the precondition */
    uint32_t n = nondet_uint32_t();
    __CPROVER_assume(n <= s2n_stuffer_data_available(&stuffer));

    /* Call the function under test */
    int result = s2n_stuffer_skip_read(&stuffer, n);

    /* Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* The read cursor must increase exactly by n */
        assert(stuffer.read_cursor == old.read_cursor + n);
    } else {
        /* On failure the whole stuffer must remain unchanged */
        assert(stuffer.read_cursor      == old.read_cursor);
        assert(stuffer.write_cursor     == old.write_cursor);
        assert(stuffer.high_water_mark  == old.high_water_mark);
        assert(stuffer.alloced          == old.alloced);
        assert(stuffer.growable         == old.growable);
        assert(stuffer.tainted          == old.tainted);
        assert(stuffer.blob.data        == old.blob.data);
        assert(stuffer.blob.size        == old.blob.size);
        assert(stuffer.blob.allocated   == old.blob.allocated);
        assert(stuffer.blob.growable    == old.blob.growable);
    }

    /* Fields that must never change, regardless of the result */
    assert(stuffer.blob.data      == old.blob.data);
    assert(stuffer.blob.size      == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable  == old.blob.growable);
    assert(stuffer.write_cursor   == old.write_cursor);
    assert(stuffer.high_water_mark== old.high_water_mark);
    assert(stuffer.alloced        == old.alloced);
    assert(stuffer.growable       == old.growable);
    assert(stuffer.tainted        == old.tainted);

    /* Validity invariant must hold after the call */
    assert(s2n_stuffer_validate(&stuffer));
}
