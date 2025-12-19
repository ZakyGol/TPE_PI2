#include "fromQueryToFile2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../htmlTable.h"
#include "queries2.h"

typedef struct {
    FILE *csv;
    htmlTable html;
} OutCtx;

static int openOut(OutCtx *ctx, const char *csvFile, const char *htmlFile,
                   int cols, const char *h1, const char *h2, const char *h3,
                   const char *h4, const char *h5)
{
    ctx->csv = fopen(csvFile, "w");
    if (!ctx->csv) return 0;

    if (cols == 3) ctx->html = newTable(htmlFile, 3, h1, h2, h3);
    else if (cols == 4) ctx->html = newTable(htmlFile, 4, h1, h2, h3, h4);
    else if (cols == 5) ctx->html = newTable(htmlFile, 5, h1, h2, h3, h4, h5);
    else { fclose(ctx->csv); ctx->csv = NULL; return 0; }

    if (!ctx->html) { fclose(ctx->csv); ctx->csv = NULL; return 0; }
    return 1;
}

static void closeOut(OutCtx *ctx){
    if (ctx->csv) fclose(ctx->csv);
    if (ctx->html) closeHTMLTable(ctx->html);
    ctx->csv = NULL;
    ctx->html = NULL;
}

/* ===== Callbacks ===== */

static void q1Cb(const char *borough, const char *type, unsigned count, void *vctx){
    OutCtx *ctx = vctx;
    char buf[32];
    snprintf(buf, sizeof buf, "%u", count);
    fprintf(ctx->csv, "%s;%s;%u\n", borough, type, count);
    addHTMLRow(ctx->html, borough, type, buf);
}

static void q2Cb(const char *borough, int hour, unsigned count, void *vctx){
    OutCtx *ctx = vctx;
    char countBuf[32], hourBuf[16];
    snprintf(countBuf, sizeof countBuf, "%u", count);
    snprintf(hourBuf, sizeof hourBuf, "%d", hour);
    fprintf(ctx->csv, "%s;%d;%u\n", borough, hour, count);
    addHTMLRow(ctx->html, borough, hourBuf, countBuf);
}

static void q3Cb(int quadLat, int quadLong, size_t distinct, void *vctx){
    OutCtx *ctx = vctx;
    char latBuf[32], longBuf[32], dBuf[32];
    snprintf(latBuf, sizeof latBuf, "%d", quadLat);
    snprintf(longBuf, sizeof longBuf, "%d", quadLong);
    snprintf(dBuf, sizeof dBuf, "%zu", distinct);
    fprintf(ctx->csv, "%d;%d;%zu\n", quadLat, quadLong, distinct);
    addHTMLRow(ctx->html, latBuf, longBuf, dBuf);
}

static void q4Cb(const char *borough, const char *agency, const char *type, int daysBetween, void *vctx){
    OutCtx *ctx = vctx;
    char buf[32];
    snprintf(buf, sizeof buf, "%d", daysBetween);
    fprintf(ctx->csv, "%s;%s;%s;%d\n", borough, agency, type, daysBetween);
    addHTMLRow(ctx->html, borough, agency, type, buf);
}

static void q5Cb(int quadLat, int quadLong, int year, int month, unsigned ytd, void *vctx){
    OutCtx *ctx = vctx;
    char latBuf[32], longBuf[32], yearBuf[16], monthBuf[16], ytdBuf[32];
    snprintf(latBuf, sizeof latBuf, "%d", quadLat);
    snprintf(longBuf, sizeof longBuf, "%d", quadLong);
    snprintf(yearBuf, sizeof yearBuf, "%d", year);
    snprintf(monthBuf, sizeof monthBuf, "%d", month);
    snprintf(ytdBuf, sizeof ytdBuf, "%u", ytd);

    fprintf(ctx->csv, "%d;%d;%d;%d;%u\n", quadLat, quadLong, year, month, ytd);
    addHTMLRow(ctx->html, latBuf, longBuf, yearBuf, monthBuf, ytdBuf);
}

/* ===== Main writer ===== */

int queriesToFile(queryADT q,
                  const char *csvFile1, const char *htmlFile1,
                  const char *csvFile2, const char *htmlFile2,
                  const char *csvFile3, const char *htmlFile3,
                  const char *csvFile4, const char *htmlFile4,
                  const char *csvFile5, const char *htmlFile5)
{
    OutCtx ctx;

    /* Q1 */
    if (!openOut(&ctx, csvFile1, htmlFile1, 3, "borough","type","requests", NULL,NULL)) return 0;
    fprintf(ctx.csv, "borough;type;requests\n");
    q1ForEach(q, q1Cb, &ctx);
    closeOut(&ctx);

    /* Q2 */
    if (!openOut(&ctx, csvFile2, htmlFile2, 3, "borough","dayHour","requests", NULL,NULL)) return 0;
    fprintf(ctx.csv, "borough;dayHour;requests\n");
    q2ForEach(q, q2Cb, &ctx);
    closeOut(&ctx);

    /* Q3 */
    if (!openOut(&ctx, csvFile3, htmlFile3, 3, "quadLat","quadLong","distinctOpenTypes", NULL,NULL)) return 0;
    fprintf(ctx.csv, "quadLat;quadLong;distinctOpenTypes\n");
    q3ForEach(q, q3Cb, &ctx);
    closeOut(&ctx);

    /* Q4 */
    if (!openOut(&ctx, csvFile4, htmlFile4, 4, "neighbourhood","agency","type","daysBetween", NULL)) return 0;
    fprintf(ctx.csv, "neighbourhood;agency;type;daysBetween\n");
    q4ForEach(q, q4Cb, &ctx);
    closeOut(&ctx);

    /* Q5 */
    if (!openOut(&ctx, csvFile5, htmlFile5, 5, "quadLat","quadLong","year","month","closedRequestsYTD")) return 0;
    fprintf(ctx.csv, "quadLat;quadLong;year;month;closedRequestsYTD\n");
    q5ForEach(q, q5Cb, &ctx);
    closeOut(&ctx);

    return 1;
}
