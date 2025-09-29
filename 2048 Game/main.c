// 2048 console (w/s/a/d controls)
// Author: generated for your GitHub pack
// Note: uses system("clear") for Linux/macOS. On Windows replace with system("cls").
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 4

int a[N][N];
int score = 0;

void clear_board() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            a[i][j] = 0;
}

void add_random() {
    int empties = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (a[i][j] == 0) empties++;
    if (!empties) return;

    int r = rand() % empties;
    int idx = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (a[i][j] == 0) {
                if (idx == r) {
                    // 90% chance of 2, 10% chance of 4
                    a[i][j] = (rand() % 10 == 0) ? 4 : 2;
                    return;
                }
                idx++;
            }
        }
    }
}

void draw_board() {
    // Clear terminal (POSIX). On Windows use "cls".
    system("clear");
    printf("Score: %d\n\n", score);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (a[i][j] == 0) printf("%6s", ".");
            else printf("%6d", a[i][j]);
        }
        printf("\n");
    }
    printf("\nControls: w (up), s (down), a (left), d (right), q (quit)\n");
}

int slide_left_once_row(int row[]) {
    // compact non-zero values to left into temp, return the new size
    int temp[N]; int idx = 0;
    for (int j = 0; j < N; ++j) if (row[j] != 0) temp[idx++] = row[j];
    for (int j = 0; j < idx; ++j) row[j] = temp[j];
    for (int j = idx; j < N; ++j) row[j] = 0;
    return idx;
}

int slide_left_row_and_merge(int row[]) {
    // returns whether any movement/merge happened in this row
    int moved = 0;
    // shift left compactly first
    int old[N]; for (int j=0;j<N;++j) old[j]=row[j];
    slide_left_once_row(row);

    for (int j = 0; j < N - 1; ++j) {
        if (row[j] != 0 && row[j] == row[j+1]) {
            row[j] *= 2;
            score += row[j];
            // shift left the remaining
            for (int k = j + 1; k < N - 1; ++k) row[k] = row[k+1];
            row[N-1] = 0;
            moved = 1;
        }
    }
    // final compact (not strictly needed each time, but safe)
    slide_left_once_row(row);
    // check if anything changed compared to old
    for (int j=0;j<N;++j) if (old[j] != row[j]) { moved = 1; break; }
    return moved;
}

int slide_left() {
    int moved = 0;
    for (int i = 0; i < N; ++i) {
        if (slide_left_row_and_merge(a[i])) moved = 1;
    }
    return moved;
}

void rotate_clockwise() {
    int b[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            b[j][N-1-i] = a[i][j];
    memcpy(a, b, sizeof a);
}

int move_dir(char dir) {
    // dir: 'a' left, 'd' right, 'w' up, 's' down
    int moved = 0;
    if (dir == 'a') moved = slide_left();
    else if (dir == 'd') {
        // right = rotate 180, left, rotate 180
        rotate_clockwise(); rotate_clockwise();
        moved = slide_left();
        rotate_clockwise(); rotate_clockwise();
    } else if (dir == 'w') {
        // up = rotate clockwise 3 times, left, rotate once
        rotate_clockwise(); rotate_clockwise(); rotate_clockwise();
        moved = slide_left();
        rotate_clockwise();
    } else if (dir == 's') {
        // down = rotate once, left, rotate 3 times
        rotate_clockwise();
        moved = slide_left();
        rotate_clockwise(); rotate_clockwise(); rotate_clockwise();
    }
    return moved;
}

int can_move() {
    // if any empty
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (a[i][j] == 0) return 1;
    // if any adjacent pairs equal horizontally or vertically
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N - 1; ++j)
            if (a[i][j] == a[i][j+1]) return 1;
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N - 1; ++i)
            if (a[i][j] == a[i+1][j]) return 1;
    return 0;
}

int has_2048() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (a[i][j] == 2048) return 1;
    return 0;
}

int main() {
    srand((unsigned)time(NULL));
    clear_board();
    add_random();
    add_random();

    while (1) {
        draw_board();
        if (has_2048()) { printf("You reached 2048! You win! 🎉\n"); break; }
        if (!can_move()) { printf("No more moves. Game over. Final score: %d\n", score); break; }

        int ch = getchar();
        // consume extra characters until newline (so input works even with enter)
        while (ch == '\n') ch = getchar();
        // after reading control char, flush rest of line
        int tmp;
        while ((tmp = getchar()) != '\n' && tmp != EOF) { /* consume */ }

        if (ch == 'q') {
            printf("Quitting. Final score: %d\n", score);
            break;
        }
        if (ch != 'a' && ch != 's' && ch != 'd' && ch != 'w') {
            // invalid key — ignore and continue
            continue;
        }
        int moved = move_dir((char)ch);
        if (moved) add_random();
    }

    return 0;
}
