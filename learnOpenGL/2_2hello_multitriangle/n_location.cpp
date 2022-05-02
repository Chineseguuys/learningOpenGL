#include <stdio.h>
#include <math.h>

static const float R = 1.0f;
#define PI 3.1415926

int main(int argc, char* argv[]) {
    if (argc <= 1) return 0;
    int N = atoi(argv[1]);

    if (N <= 2 || N >= 100) return 0;

    for (int i = 0; i < N; ++i) {
        printf("%.2f, %.2f, 0.0,\n", R * cos(2 * PI * i / N), R * sin(2 * PI * i / N));
    }

    return 0;
}