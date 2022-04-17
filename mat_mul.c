#include <stdio.h>

int main(void)
{
    int A_C1[4],
        A_C2[4],
        A_C3[4],
        A_C4[4],
    int B_R1[4],
        B_R2[4],
        B_R3[4],
        B_R4[4],
    // input
    for (int i = 0; i < 4; i++) std::cin >> A_C1[i];
    for (int i = 0; i < 4; i++) std::cin >> A_C2[i];
    for (int i = 0; i < 4; i++) std::cin >> A_C3[i];
    for (int i = 0; i < 4; i++) std::cin >> A_C4[i];

    for (int i = 0; i < 4; i++) std::cin >> B_R1[i];
    for (int i = 0; i < 4; i++) std::cin >> B_R2[i];
    for (int i = 0; i < 4; i++) std::cin >> B_R3[i];
    for (int i = 0; i < 4; i++) std::cin >> B_R4[i];

    for (int i = 0; i < 4; i++) {
        int tmp = 0;
        for (int j = 0; j < 4; j++) {
            tmp += A_C1[i] * B_R1[j]
            tmp += A_C2[i] * B_R2[j]
            tmp += A_C3[i] * B_R3[j]
            tmp += A_C4[i] * B_R4[j]
        }
    }
}
