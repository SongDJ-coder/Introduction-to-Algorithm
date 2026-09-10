#ifndef THREE_WAY_SELECTION_H
#define THREE_WAY_SELECTION_H

#include <stddef.h>


/* SELECT — median of medians. 최악에도 Theta(n).
 *
 *   i 는 **1-based** (i = 1 이 최솟값).  C 배열의 0-based 와 다르다.
 *   반환  0 성공 / 0 이 아닌 값 실패.
 *   값은 *result 로 나온다.
 *   arr 을 **재배치한다** (qsort 방식). 원본이 필요하면 호출자가 복사해서 넘긴다. */
int three_way_select(int *arr, size_t n, int i, int *result);


/* A[p..r] 에서 i 번째로 작은 값을 반환 (i 는 1-based, 구간 기준).
 * 재귀 본체. 유효한 i 가 들어온다고 전제한다 — 검증은 three_way_select 가 한다. */
int three_way(int *arr, int p, int r, int i);


#endif
