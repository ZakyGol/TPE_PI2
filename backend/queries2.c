#include "queries2.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(CITY_NYC)
    #define CERRADO "Closed"
#elif defined(CITY_CHICAGO)
    #define CERRADO "Completed"
#else
    #error "Debe definirse CITY_NYC o CITY_CHICAGO"
#endif

/* ===== strdup portable (evita warnings pedantic) ===== */
static char *strdup_c99(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

/* ===== Fecha a “días desde epoch” (para restar) ===== */
static int days_from_civil(int y, int m, int d) {
    y -= m <= 2;
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

typedef struct boroughTypes {
    char *type;
    unsigned int count;
    struct boroughTypes *next;
} boroughTypes;

typedef struct q1Node {
    char *borough;
    boroughTypes *firstType;
    struct q1Node *next;
} q1Node;

typedef q1Node *q1List;

typedef struct q2Node {
    char *borough;
    unsigned int count[HOURS];
    struct q2Node *next;
} q2Node;

typedef q2Node *q2List;

typedef struct latLongs {
    int quadLong;
    char **codes;      /* códigos distintos (Open) */
    size_t dim;        /* cantidad de códigos */
    size_t cap;        /* capacidad reservada */
    struct latLongs *next;
} latLongs;

typedef struct q3Node {
    int quadLat;
    latLongs *firstLong;
    struct q3Node *next;
} q3Node;

typedef q3Node *q3List;

typedef struct agencyTypes {
    char *type;
    int recDay;
    int oldDay;
    struct agencyTypes *next;
} agencyTypes;

typedef struct boroughAg {
    char *agency;
    agencyTypes *firstType;
    struct boroughAg *next;
} boroughAg;

typedef struct q4Node {
    char *borough;
    boroughAg *firstAgency;
    struct q4Node *next;
} q4Node;

typedef q4Node *q4List;

typedef struct quadYearMonth {
    int month;
    unsigned int count;      /* cantidad cerrados en ese mes */
    struct quadYearMonth *next;
} quadYearMonth;

typedef struct quadYear {
    int year;
    quadYearMonth *firstMonth;
    struct quadYear *next;
} quadYear;

typedef struct latLongs5 {
    int quadLong;
    quadYear *firstYear;
    struct latLongs5 *next;
} latLongs5;

typedef struct q5Node {
    int quadLat;
    latLongs5 *firstLong;
    struct q5Node *next;
} q5Node;

typedef q5Node *q5List;

struct queryCDT {
    q1List q1;
    q2List q2;
    q3List q3;
    q4List q4;
    q5List q5;
};

/* =========================
   HELPERS ORDENADOS (iterativos)
   ========================= */

/* ---------- Q1 helpers ---------- */
static boroughTypes *q1_insertType(boroughTypes *head, const char *typeName) {
    boroughTypes *prev = NULL, *cur = head;
    while (cur && strcmp(cur->type, typeName) < 0) {
        prev = cur;
        cur = cur->next;
    }
    if (cur && strcmp(cur->type, typeName) == 0) {
        cur->count++;
        return head;
    }
    boroughTypes *node = (boroughTypes *)malloc(sizeof(*node));
    if (!node) return head;
    node->type = strdup_c99(typeName);
    if (!node->type) { free(node); return head; }
    node->count = 1;
    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

static q1List q1_getOrInsertBorough(q1List head, const char *borough, const char *typeName) {
    q1Node *prev = NULL, *cur = head;
    while (cur && strcmp(cur->borough, borough) < 0) {
        prev = cur;
        cur = cur->next;
    }
    if (cur && strcmp(cur->borough, borough) == 0) {
        cur->firstType = q1_insertType(cur->firstType, typeName);
        return head;
    }

    q1Node *node = (q1Node *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->borough = strdup_c99(borough);
    if (!node->borough) { free(node); return head; }

    node->firstType = q1_insertType(NULL, typeName);
    if (!node->firstType) { free(node->borough); free(node); return head; }

    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

/* ---------- Q2 helpers ---------- */
static q2List q2_getOrInsertBorough(q2List head, const char *borough, int hour) {
    q2Node *prev = NULL, *cur = head;
    while (cur && strcmp(cur->borough, borough) < 0) {
        prev = cur;
        cur = cur->next;
    }
    if (cur && strcmp(cur->borough, borough) == 0) {
        cur->count[hour]++;
        return head;
    }

    q2Node *node = (q2Node *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->borough = strdup_c99(borough);
    if (!node->borough) { free(node); return head; }
    node->count[hour] = 1;
    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

/* ---------- Q3 helpers ---------- */
static int q3_codeExists(latLongs *cell, const char *code) {
    for (size_t i = 0; i < cell->dim; i++)
        if (strcmp(cell->codes[i], code) == 0) return 1;
    return 0;
}

static void q3_addCode(latLongs *cell, const char *code) {
    if (q3_codeExists(cell, code)) return;

    if (cell->dim == cell->cap) {
        size_t newCap = cell->cap + BLOQUE;
        char **tmp = (char **)realloc(cell->codes, newCap * sizeof(char *));
        if (!tmp) return; /* si falla, no agregamos */
        cell->codes = tmp;
        cell->cap = newCap;
    }
    cell->codes[cell->dim] = strdup_c99(code);
    if (!cell->codes[cell->dim]) return;
    cell->dim++;
}

static latLongs *q3_getOrInsertLong(latLongs *head, int quadLong, const char *code) {
    latLongs *prev = NULL, *cur = head;
    while (cur && cur->quadLong < quadLong) {
        prev = cur;
        cur = cur->next;
    }
    if (cur && cur->quadLong == quadLong) {
        q3_addCode(cur, code);
        return head;
    }

    latLongs *node = (latLongs *)calloc(1, sizeof(*node));
    if (!node) return head;

    node->quadLong = quadLong;
    node->cap = BLOQUE;
    node->codes = (char **)malloc(node->cap * sizeof(char *));
    if (!node->codes) { free(node); return head; }

    node->codes[0] = strdup_c99(code);
    if (!node->codes[0]) { free(node->codes); free(node); return head; }
    node->dim = 1;

    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

static q3List q3_getOrInsertLat(q3List head, int quadLat, int quadLong, const char *code) {
    q3Node *prev = NULL, *cur = head;
    while (cur && cur->quadLat < quadLat) {
        prev = cur;
        cur = cur->next;
    }
    if (cur && cur->quadLat == quadLat) {
        cur->firstLong = q3_getOrInsertLong(cur->firstLong, quadLong, code);
        return head;
    }

    q3Node *node = (q3Node *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->quadLat = quadLat;
    node->firstLong = q3_getOrInsertLong(NULL, quadLong, code);
    if (!node->firstLong) { free(node); return head; }

    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

/* ---------- Q4 helpers ---------- */
static agencyTypes *q4_insertType(agencyTypes *head, const char *typeName, int dateDay) {
    agencyTypes *prev = NULL, *cur = head;
    while (cur && strcmp(cur->type, typeName) < 0) {
        prev = cur; cur = cur->next;
    }
    if (cur && strcmp(cur->type, typeName) == 0) {
        if (dateDay > cur->recDay) cur->recDay = dateDay;
        if (dateDay < cur->oldDay) cur->oldDay = dateDay;
        return head;
    }

    agencyTypes *node = (agencyTypes *)malloc(sizeof(*node));
    if (!node) return head;
    node->type = strdup_c99(typeName);
    if (!node->type) { free(node); return head; }
    node->recDay = node->oldDay = dateDay;
    node->next = cur;

    if (!prev) return node;
    prev->next = node;
    return head;
}

static boroughAg *q4_getOrInsertAgency(boroughAg *head, const char *agency, const char *typeName, int dateDay) {
    boroughAg *prev = NULL, *cur = head;
    while (cur && strcmp(cur->agency, agency) < 0) {
        prev = cur; cur = cur->next;
    }
    if (cur && strcmp(cur->agency, agency) == 0) {
        cur->firstType = q4_insertType(cur->firstType, typeName, dateDay);
        return head;
    }

    boroughAg *node = (boroughAg *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->agency = strdup_c99(agency);
    if (!node->agency) { free(node); return head; }

    node->firstType = q4_insertType(NULL, typeName, dateDay);
    if (!node->firstType) { free(node->agency); free(node); return head; }

    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

static q4List q4_getOrInsertBorough(q4List head, const char *borough, const char *agency, const char *typeName, int dateDay) {
    q4Node *prev = NULL, *cur = head;
    while (cur && strcmp(cur->borough, borough) < 0) {
        prev = cur; cur = cur->next;
    }
    if (cur && strcmp(cur->borough, borough) == 0) {
        cur->firstAgency = q4_getOrInsertAgency(cur->firstAgency, agency, typeName, dateDay);
        return head;
    }

    q4Node *node = (q4Node *)calloc(1, sizeof(*node));
    if (!node) return head;

    node->borough = strdup_c99(borough);
    if (!node->borough) { free(node); return head; }

    node->firstAgency = q4_getOrInsertAgency(NULL, agency, typeName, dateDay);
    if (!node->firstAgency) { free(node->borough); free(node); return head; }

    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

/* ---------- Q5 helpers ---------- */
static quadYearMonth *q5_insertMonth(quadYearMonth *head, int month) {
    quadYearMonth *prev = NULL, *cur = head;
    while (cur && cur->month < month) { prev = cur; cur = cur->next; }
    if (cur && cur->month == month) {
        cur->count++;
        return head;
    }
    quadYearMonth *node = (quadYearMonth *)malloc(sizeof(*node));
    if (!node) return head;
    node->month = month;
    node->count = 1;
    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

static quadYear *q5_getOrInsertYear(quadYear *head, int year, int month) {
    quadYear *prev = NULL, *cur = head;
    while (cur && cur->year < year) { prev = cur; cur = cur->next; }
    if (cur && cur->year == year) {
        cur->firstMonth = q5_insertMonth(cur->firstMonth, month);
        return head;
    }
    quadYear *node = (quadYear *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->year = year;
    node->firstMonth = q5_insertMonth(NULL, month);
    if (!node->firstMonth) { free(node); return head; }
    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

static latLongs5 *q5_getOrInsertLong(latLongs5 *head, int quadLong, int year, int month) {
    latLongs5 *prev = NULL, *cur = head;
    while (cur && cur->quadLong < quadLong) { prev = cur; cur = cur->next; }
    if (cur && cur->quadLong == quadLong) {
        cur->firstYear = q5_getOrInsertYear(cur->firstYear, year, month);
        return head;
    }
    latLongs5 *node = (latLongs5 *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->quadLong = quadLong;
    node->firstYear = q5_getOrInsertYear(NULL, year, month);
    if (!node->firstYear) { free(node); return head; }
    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

static q5List q5_getOrInsertLat(q5List head, int quadLat, int quadLong, int year, int month) {
    q5Node *prev = NULL, *cur = head;
    while (cur && cur->quadLat < quadLat) { prev = cur; cur = cur->next; }
    if (cur && cur->quadLat == quadLat) {
        cur->firstLong = q5_getOrInsertLong(cur->firstLong, quadLong, year, month);
        return head;
    }
    q5Node *node = (q5Node *)calloc(1, sizeof(*node));
    if (!node) return head;
    node->quadLat = quadLat;
    node->firstLong = q5_getOrInsertLong(NULL, quadLong, year, month);
    if (!node->firstLong) { free(node); return head; }
    node->next = cur;
    if (!prev) return node;
    prev->next = node;
    return head;
}

/* =========================
   API PÚBLICA
   ========================= */

queryADT newQueries(void) {
    return (queryADT)calloc(1, sizeof(struct queryCDT));
}

/* Ingesta: un reclamo → actualiza las 5 queries */
void addToQueries(queryADT q,
                  const char *agency,
                  const char *code,
                  const char *typeName,
                  const char *status,
                  const char *borough,
                  int year, int month, int day, int hour,
                  double lat, double lon,
                  int yMax, int yMin) {
    if (!q || !status || !borough || !typeName) return;

    /* Validaciones mínimas */
    if (hour < 0 || hour >= HOURS) return;
    if (month < 1 || month > 12) return;
    if (lat < -90.0 || lat > 90.0) return;
    if (lon < -180.0 || lon > 180.0) return;

    int quadLat = (int)(lat * 10.0);
    int quadLong = (int)(lon * 10.0);

    int isClosed = (strcmp(status, CERRADO) == 0);

    /* Q1 siempre */
    q->q1 = q1_getOrInsertBorough(q->q1, borough, typeName);

    /* Q2: solo cerrados */
    if (isClosed) {
        q->q2 = q2_getOrInsertBorough(q->q2, borough, hour);
    }

    /* Q3: solo abiertos */
    if (strcmp(status, "Open") == 0 && code) {
        q->q3 = q3_getOrInsertLat(q->q3, quadLat, quadLong, code);
    }

    /* Q4/Q5: solo si año está en rango (estricto como venías usando) */
    if (year > yMin && year < yMax) {
        int dday = days_from_civil(year, month, day);

        if (agency) {
            q->q4 = q4_getOrInsertBorough(q->q4, borough, agency, typeName, dday);
        }

        if (isClosed) {
            q->q5 = q5_getOrInsertLat(q->q5, quadLat, quadLong, year, month);
        }
    }
}

/* =========================
   ForEach (CALLBACKS)
   ========================= */

void q1ForEach(const queryADT q, q1RowFn fn, void *ctx) {
    if (!q || !fn) return;
    for (q1List b = q->q1; b; b = b->next)
        for (boroughTypes *t = b->firstType; t; t = t->next)
            fn(ctx, b->borough, t->type, t->count);
}

void q2ForEach(const queryADT q, q2RowFn fn, void *ctx) {
    if (!q || !fn) return;
    for (q2List b = q->q2; b; b = b->next)
        for (int h = 0; h < HOURS; h++)
            fn(ctx, b->borough, h, b->count[h]);
}

void q3ForEach(const queryADT q, q3RowFn fn, void *ctx) {
    if (!q || !fn) return;
    for (q3List lat = q->q3; lat; lat = lat->next)
        for (latLongs *lo = lat->firstLong; lo; lo = lo->next)
            fn(ctx, lat->quadLat, lo->quadLong, lo->dim);
}

void q4ForEach(const queryADT q, q4RowFn fn, void *ctx) {
    if (!q || !fn) return;
    for (q4List b = q->q4; b; b = b->next) {
        for (boroughAg *ag = b->firstAgency; ag; ag = ag->next) {
            for (agencyTypes *t = ag->firstType; t; t = t->next) {
                fn(ctx, b->borough, ag->agency, t->type, (t->recDay - t->oldDay));
            }
        }
    }
}

/* Q5: el backend ya calcula YTD ordenado por mes */
void q5ForEach(const queryADT q, q5RowFn fn, void *ctx) {
    if (!q || !fn) return;
    for (q5List lat = q->q5; lat; lat = lat->next) {
        for (latLongs5 *lo = lat->firstLong; lo; lo = lo->next) {
            for (quadYear *y = lo->firstYear; y; y = y->next) {
                unsigned int ytd = 0;
                for (quadYearMonth *m = y->firstMonth; m; m = m->next) {
                    ytd += m->count;
                    fn(ctx, lat->quadLat, lo->quadLong, y->year, m->month, ytd);
                }
            }
        }
    }
}

/* =========================
   Liberación de memoria
   ========================= */

static void freeQ1(q1List q1) {
    while (q1) {
        q1Node *nx = q1->next;
        free(q1->borough);
        boroughTypes *t = q1->firstType;
        while (t) {
            boroughTypes *tnx = t->next;
            free(t->type);
            free(t);
            t = tnx;
        }
        free(q1);
        q1 = nx;
    }
}

static void freeQ2(q2List q2) {
    while (q2) {
        q2Node *nx = q2->next;
        free(q2->borough);
        free(q2);
        q2 = nx;
    }
}

static void freeQ3(q3List q3) {
    while (q3) {
        q3Node *nx = q3->next;
        latLongs *lo = q3->firstLong;
        while (lo) {
            latLongs *lnx = lo->next;
            for (size_t i = 0; i < lo->dim; i++) free(lo->codes[i]);
            free(lo->codes);
            free(lo);
            lo = lnx;
        }
        free(q3);
        q3 = nx;
    }
}

static void freeQ4(q4List q4) {
    while (q4) {
        q4Node *nx = q4->next;
        free(q4->borough);
        boroughAg *ag = q4->firstAgency;
        while (ag) {
            boroughAg *agnx = ag->next;
            free(ag->agency);
            agencyTypes *t = ag->firstType;
            while (t) {
                agencyTypes *tnx = t->next;
                free(t->type);
                free(t);
                t = tnx;
            }
            free(ag);
            ag = agnx;
        }
        free(q4);
        q4 = nx;
    }
}

static void freeQ5(q5List q5) {
    while (q5) {
        q5Node *nx = q5->next;
        latLongs5 *lo = q5->firstLong;
        while (lo) {
            latLongs5 *lnx = lo->next;
            quadYear *y = lo->firstYear;
            while (y) {
                quadYear *ynx = y->next;
                quadYearMonth *m = y->firstMonth;
                while (m) {
                    quadYearMonth *mnx = m->next;
                    free(m);
                    m = mnx;
                }
                free(y);
                y = ynx;
            }
            free(lo);
            lo = lnx;
        }
        free(q5);
        q5 = nx;
    }
}

void freeQueries(queryADT q) {
    if (!q) return;
    freeQ1(q->q1);
    freeQ2(q->q2);
    freeQ3(q->q3);
    freeQ4(q->q4);
    freeQ5(q->q5);
    free(q);
}