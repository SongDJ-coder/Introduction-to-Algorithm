# ch9_2 — RANDOMIZED-SELECT

## 재사용

ch7 코드를 **복사하지 않는다.** 컴파일 시 소스를 같이 넘겨 링크한다.

```c
#include "../../ch7/quicksort.h"
```

쓰는 것:

```c
int partition(int arr[], int p, int r);              // pivot 최종 위치 반환
int randomized_partition(int arr[], int p, int r);   // 무작위 pivot을 r로 swap 후 partition
```

`quicksort.h`의 나머지 선언(quicksort, hoare_*, reset_depth 등)은 링크만 되고 안 쓴다.

## 빌드

```bash
cd ~/Projects/Algorithm/CLRS/ch9/ch9_2

# 정확성 (ASan 켬)
gcc -g -O0 -Wall -Wextra -fsanitize=address \
    ../../ch7/quicksort.c randomized_select.c main.c \
    -o ../../bin/randomized_select && ../../bin/randomized_select

# 시간 측정 (ASan 끔 — 8.3에서 얻은 교훈)
gcc -O2 -Wall ../../ch7/quicksort.c randomized_select.c main.c \
    -o ../../bin/randomized_select && ../../bin/randomized_select
```

## 비용 검증 (별도 하네스)

정확성 테스트는 Θ(n lg n) 구현과 구별하지 못한다. `main_cost.c`가 그걸 검사한다.
**ASan 끄고 `-O2`로 돌린다** — 비교 횟수는 ASan과 무관하지만 훨씬 빠르다.

```bash
gcc -O2 -Wall -Wextra ../../ch7/quicksort.c randomized_select.c main_cost.c \
    -o ../../bin/select_cost && ../../bin/select_cost
```

계측은 `ch7/quicksort.c`의 `partition` 안에 있다 (`reset_comparisons` / `get_comparisons`).
`reset_depth` / `get_max_depth`와 같은 패턴.

## 파일

| 파일 | 내용 | 작성 |
|---|---|---|
| `randomized_select.h` | 공개 선언 | 학습자 |
| `randomized_select.c` | 알고리즘 + static 재귀 헬퍼 | 학습자 |
| `main.c` | 정확성 하네스 (14172 케이스) | Claude |
| `main_cost.c` | 비용 검증 하네스 | Claude |

## 주의

- **`select`라는 이름을 쓰지 않는다.** POSIX `<sys/select.h>`의 `select()`와 충돌한다.
- `srand()` 위치를 정해야 한다. `quicksort.c`는 부르지 않는다.
- `partition`은 배열을 재배치한다. 시그니처에 `const`를 붙일 수 없다.
