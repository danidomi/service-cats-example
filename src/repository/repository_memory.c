#include "repository.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct CatNode {
    int id;
    int age;
    char *name;
    struct CatNode *next;
} CatNode;

static CatNode *head = NULL;
static int next_id = 1;
static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

static void seed_locked(void) {
    static const struct { const char *name; int age; } seed[] = {
        {"Whiskers", 3},
        {"Mittens",  5},
        {"Felix",    1},
    };
    for (size_t i = 0; i < sizeof(seed) / sizeof(seed[0]); i++) {
        CatNode *n = calloc(1, sizeof(CatNode));
        if (!n) return;
        n->id = next_id++;
        n->name = strdup(seed[i].name);
        n->age = seed[i].age;
        n->next = head;
        head = n;
    }
}

void repository_init(const DbConfig *cfg) {
    (void)cfg; /* memory backend has no connection to configure */

    pthread_mutex_lock(&mu);
    if (head == NULL) seed_locked();
    int seeded = next_id - 1;
    pthread_mutex_unlock(&mu);

    log_message(INFO, "in-memory repository ready (seeded %d cats)", seeded);
}

void repository_close(void) {
    pthread_mutex_lock(&mu);
    while (head) {
        CatNode *n = head;
        head = head->next;
        free(n->name);
        free(n);
    }
    next_id = 1;
    pthread_mutex_unlock(&mu);
}

static Cat *clone_locked(const CatNode *n) {
    Cat *out = malloc(sizeof(Cat));
    if (!out) return NULL;
    out->id = n->id;
    out->age = n->age;
    out->name = n->name ? strdup(n->name) : NULL;
    out->error = NULL;
    return out;
}

Cat *find_cat(int id, Error **err) {
    (void)err; /* memory lookup has no I/O failure mode */

    pthread_mutex_lock(&mu);
    Cat *out = NULL;
    for (CatNode *n = head; n; n = n->next) {
        if (n->id == id) {
            out = clone_locked(n);
            break;
        }
    }
    pthread_mutex_unlock(&mu);
    return out;
}

Cat *persist_cat(int age, char *name, Error **err) {
    CatNode *n = calloc(1, sizeof(CatNode));
    if (!n) {
        if (err && !*err) *err = error_new("out of memory");
        return NULL;
    }

    pthread_mutex_lock(&mu);
    n->id = next_id++;
    n->age = age;
    n->name = name ? strdup(name) : NULL;
    n->next = head;
    head = n;
    Cat *out = clone_locked(n);
    pthread_mutex_unlock(&mu);

    if (!out && err && !*err) *err = error_new("out of memory");
    return out;
}

static int cmp_cat_by_id(const void *a, const void *b) {
    int ia = (*(Cat *const *)a)->id;
    int ib = (*(Cat *const *)b)->id;
    return (ia > ib) - (ia < ib);
}

Cat **list_cats(size_t *count, Error **err) {
    if (count) *count = 0;
    (void)err;

    pthread_mutex_lock(&mu);
    size_t n = 0;
    for (CatNode *p = head; p; p = p->next) n++;
    if (n == 0) {
        pthread_mutex_unlock(&mu);
        return NULL;
    }
    Cat **arr = malloc(n * sizeof(Cat *));
    if (!arr) {
        pthread_mutex_unlock(&mu);
        if (err && !*err) *err = error_new("out of memory");
        return NULL;
    }
    size_t i = 0;
    for (CatNode *p = head; p && i < n; p = p->next) {
        arr[i] = clone_locked(p);
        if (!arr[i]) {
            for (size_t j = 0; j < i; j++) {
                free(arr[j]->name);
                free(arr[j]);
            }
            free(arr);
            pthread_mutex_unlock(&mu);
            if (err && !*err) *err = error_new("out of memory");
            return NULL;
        }
        i++;
    }
    pthread_mutex_unlock(&mu);

    qsort(arr, n, sizeof(Cat *), cmp_cat_by_id);
    if (count) *count = n;
    return arr;
}

void cats_free(Cat **cats, size_t count) {
    if (!cats) return;
    for (size_t i = 0; i < count; i++) {
        if (!cats[i]) continue;
        free(cats[i]->name);
        free(cats[i]);
    }
    free(cats);
}

Cat *update_cat(int id, int age, char *name, Error **err) {
    (void)err;
    pthread_mutex_lock(&mu);
    Cat *out = NULL;
    for (CatNode *p = head; p; p = p->next) {
        if (p->id == id) {
            free(p->name);
            p->name = name ? strdup(name) : NULL;
            p->age = age;
            out = clone_locked(p);
            break;
        }
    }
    pthread_mutex_unlock(&mu);
    return out;
}

int delete_cat(int id, Error **err) {
    (void)err;
    pthread_mutex_lock(&mu);
    CatNode *prev = NULL;
    for (CatNode *p = head; p; prev = p, p = p->next) {
        if (p->id == id) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name);
            free(p);
            pthread_mutex_unlock(&mu);
            return 1;
        }
    }
    pthread_mutex_unlock(&mu);
    return 0;
}
