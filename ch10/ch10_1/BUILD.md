# ch10_1 — Stacks and Queues (CLRS 10.1)

## 파일

| 파일 | 누가 | 내용 |
|---|---|---|
| `stack.h` / `stack.c` | 학습자 | 배열 스택. `top` 은 **원소 개수** |
| `queue.h` / `queue.c` | 학습자 | 링 버퍼 큐. **한 칸 버리기** |
| `main_stack.c` | Claude | 스택 동작 확인 (연습문제 10.1-1 순서) |
| `main_queue.c` | Claude | 큐 테스트 하네스 — 26 케이스 |

두 자료구조는 독립이다. 서로를 링크하지 않는다.

## 빌드

```bash
cd ~/Projects/Algorithm/CLRS/ch10/ch10_1

# 스택
gcc -g -O0 -Wall -Wextra -fsanitize=address stack.c main_stack.c \
    -o ../../bin/stack && ../../bin/stack

# 큐
gcc -g -O0 -Wall -Wextra -fsanitize=address queue.c main_queue.c \
    -o ../../bin/queue && ../../bin/queue
```

정확성은 ASan 켜고, 시간 측정은 끄고 `-O2` 로 다시 컴파일한다 (8.3 상 문제의 교훈).

## 현재 상태

```
스택   기능 완료. 데모로 확인
큐     26 / 26 PASS, ASan 누수 0
```

## 관례

| 항목 | 결정 |
|---|---|
| 반환 | `0` 성공 / `0` 아닌 값 실패. 값은 out-param |
| 예외 | `*_empty` 만 **`1` 이 참**(비었음). 상태 질의라서 |
| 배열 소유권 | **호출자가 준비.** `set_stack` / `set_queue` 로 꽂는다 |
| `top` | **원소 개수.** 맨 위 원소는 `arr[top-1]`, 다음에 쓸 칸은 `arr[top]` |
| `head` / `tail` | 다음에 뺄 칸 / 다음에 넣을 칸. 항상 `0 <= x < capacity` |
| 큐 용량 | `capacity - 1` (한 칸 버리기) |

### 링 버퍼 불변식

**`head` 와 `tail` 은 항상 `0` 이상 `capacity` 미만이다.**
지키는 곳은 갱신하는 두 줄뿐 — `(x + 1) % capacity`.

그래서 판정식의 `%` 는 **`+1` 이 붙은 쪽에만** 필요하다.
`head % capacity` 는 자기 자신이 나오는 죽은 연산이고, 남겨두면 거짓말이 된다.

```
비었다      head == tail
가득 찼다   (tail + 1) % capacity == head
```

## 남은 것

- [ ] `queue_push` / `queue_pop` → `enqueue` / `dequeue` (관례)
- [ ] 타입 통일 — `Stack` 은 `size_t`, `Queue` 는 `int`
- [ ] `set_stack` 에 NULL 검사 (큐엔 있고 스택엔 없다 — 대칭 깨짐)
- [ ] `main_stack.c` 를 무작위 스트레스 하네스로 (지금은 데모 수준)
- [ ] `size == 1` 을 허용할지 — 크래시는 없지만 용량 0 인 큐가 된다
- [ ] 연습문제 10.1-2(배열 하나에 스택 둘) · 10.1-5(deque) ·
      10.1-6(스택 2개로 큐) · 10.1-7(큐 2개로 스택)
- [x] 10.1-1 · 10.1-3 — 하네스에서 확인
- [x] 10.1-4 (underflow/overflow 검사) — 처음부터 그렇게 구현
