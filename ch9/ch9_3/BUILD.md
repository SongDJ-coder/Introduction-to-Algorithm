# ch9_3 — SELECT (median of medians, 최악 선형)

## 알고리즘

```
SELECT(A, p, r, i)

1  A[p..r] 의 n 개를 5개씩 ⌈n/5⌉개 그룹으로 나눈다 (마지막은 n mod 5 개)
2  각 그룹을 정렬해 중앙값을 꺼낸다.  →  중앙값 ⌈n/5⌉개
3  그 중앙값들에 SELECT 를 재귀 호출해 그들의 중앙값 x 를 구한다
4  x 를 pivot 으로 PARTITION.  k = (작은 쪽 개수) + 1
5  i == k  ->  return x
   i <  k  ->  왼쪽에서 i번째를 재귀
   i >  k  ->  오른쪽에서 (i-k)번째를 재귀
```

보장: `x` 이하가 최소 `3n/10 - 6`개, 이상도 최소 `3n/10 - 6`개
→ 재귀로 가는 쪽은 최대 `7n/10 + 6`

```
T(n) ≤ T(n/5) + T(7n/10 + 6) + O(n),     1/5 + 7/10 = 9/10 < 1   ->   Θ(n)
```

## 확정한 설계 결정

| 항목 | 결정 | 근거 |
|---|---|---|
| partition | **3-way 직접 작성** (`< x` / `== x` / `> x`) | Lomuto 는 `<=` 라 중복을 전부 왼쪽으로 보낸다. 중복 밀도가 30% 를 넘으면 `7n/10` 보장이 깨져 Θ(n²) |
| x 전달 | **값으로 인자에 받는다** | 직접 짜므로 `arr[r]` 제약이 없다. **x 의 인덱스를 찾는 선형 탐색이 통째로 사라진다** |
| 중앙값 모으기 | **원본 배열 앞쪽으로 swap (in-place)** | malloc 을 재귀 안에서 겹치면 실패 경로와 free 관리가 단계마다 생긴다. 추가 메모리 O(1) |
| 입력 훼손 | 재배치 허용 (qsort 방식) | 9.2 와 동일 |
| `i` 기수 | 1-based, 변환은 진입점 한 곳에서 | 9.2 와 동일 |
| 구조 | 공개 진입점 + static 헬퍼들 | 9.2 와 동일 |
| 반환 | `0` 성공 / `0` 아닌 값 실패, 값은 out-parameter | 8.3 · 9.1 · 9.2 와 통일 |

> [!important] ch7 `partition` 을 쓰지 않는다
> 9.2 와 달리 여기서는 재사용하지 않는다. pivot 을 밖에서 지정해야 하고 3-way 가 필요하기 때문.
> 다만 계측(`reset_comparisons` / `get_comparisons`)은 ch7 안에 있으므로, 비용 검증을 하려면
> **9.3 자체 partition 에도 같은 계측을 붙이거나 별도 카운터를 둬야 한다.**

## 5행 판정 (3-way 로 바뀐 부분)

```
k_lo = lo - p + 1        // == x 인 것들 중 가장 작은 순위
k_hi = hi - p + 1        // 가장 큰 순위

k_lo <= i <= k_hi   ->  return x
i < k_lo            ->  SELECT(A, p,    lo-1, i)            // i 그대로
i > k_hi            ->  SELECT(A, hi+1, r,    i - k_hi)     // 왼쪽에 남기고 가는 개수를 뺀다
```

9.2 의 `i - k` 와 구조가 같다. 잘려나가는 것이 "작은 쪽 + pivot 하나"에서
"작은 쪽 + `x` 와 같은 것 전부"로 바뀌었을 뿐.

## 함수 목록 (이름·시그니처는 학습자가 정한다)

| 역할 | 설명 |
|---|---|
| 공개 진입점 | 배열·크기·순위. 검증과 경계 변환 |
| (static) 재귀 SELECT | `p`, `r`, `i` 로 도는 본체 |
| (static) 3-way partition | 값 `x` 를 받아 세 구역으로 가르고 두 경계를 out-parameter 로 |
| (static) 그룹 중앙값 모으기 | 1·2행. 5개씩 정렬해 중앙값을 앞쪽으로 swap. 모은 개수 반환 |
| (static) 작은 구간 정렬 | 원소 5개 이하. 삽입 정렬 |

## 구현하며 부딪힐 것

- 마지막 그룹이 5개가 안 될 때 중앙값을 어디로 잡나
- 3-way partition 의 루프 불변식 — 세 구역이 자라는 방식
- 3행 재귀에 넘길 `i`(= 중앙값들의 중앙값 순위)는 얼마인가
- 기저 사례 — 원소가 몇 개 이하일 때 그냥 정렬해버릴 것인가

## 빌드

```bash
cd ~/Projects/Algorithm/CLRS/ch9/ch9_3

# 정확성 (ASan 켬)
gcc -g -O0 -Wall -Wextra -fsanitize=address \
    select.c main.c -o ../../bin/select_worst && ../../bin/select_worst

# 비용 측정 (ASan 끔, -O2)
gcc -O2 -Wall -Wextra select.c main_cost.c \
    -o ../../bin/select_worst_cost && ../../bin/select_worst_cost
```

ch7 코드가 필요해지면 `../../ch7/quicksort.c` 를 컴파일 목록에 추가한다.
9.2 와 성능을 비교하려면 `../ch9_2/randomized_select.c` 도 함께 넘긴다.

## 주의

- **`select` 라는 이름을 쓰지 않는다.** POSIX `<sys/select.h>` 의 `select()` 와 충돌.
  파일명 `select.c` 는 괜찮지만 **함수 이름**은 피한다
- 최악 Θ(n) 이지만 **상수가 크다.** `O(n)` 항만 2n(그룹 정렬) + n(partition),
  두 재귀까지 합치면 ~10n 규모. 9.2 는 `i=n/2` 에서 3.4n 이었다.
  **평균적으로는 9.2 가 3배쯤 빠르다.** 9.3 이 파는 건 속도가 아니라 보장이다
