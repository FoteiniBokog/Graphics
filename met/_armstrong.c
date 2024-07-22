#include <stdio.h>

int main(){
int num ,sum ,temp ,digit ,a ,b ,T_0 ,T_1 ,T_2 ,T_3 ,T_4;
	L_1: ; //armstrong start // (begin_block, armstrong, _, _)
	L_2: printf("Enter variable input for:num\n"); scanf("%d", &num); // (inp, num, _, _)
	L_3: temp = num; // (:=, num, _, temp)
	L_4: if(temp > 0) goto L_6; // (>, temp, 0, 6)
	L_5: goto L_20; // (jump, _, _, 20)
	L_6: T_0 = temp/10; // (/, temp, 10, T_0)
	L_7: digit = temp; // (:=, temp, _, digit)
	L_8: T_1 = digit*digit; // (*, digit, digit, T_1)
	L_9: T_2 = digit*digit; // (*, digit, digit, T_2)
	L_10: T_3 = sum+T_2; // (+, sum, T_2, T_3)
	L_11: sum = T_3; // (:=, T_3, _, sum)
	L_12: T_4 = temp/10; // (/, temp, 10, T_4)
	L_13: temp = temp; // (:=, temp, _, temp)
	L_14: if(num == 0) goto L_16; // (=, num, 0, 16)
	L_15: goto L_18; // (jump, _, _, 18)
	L_16: printf("Outputnum= %d\n", num); // (out, num, _, _)
	L_17: goto L_19; // (jump, _, _, 19)
	L_18: printf("Outputnum= %d\n", num); // (out, num, _, _)
	L_19: goto L_4; // (jump, _, _, 4)
	L_20: ; //armstrong end // (end_block, armstrong, _, _)
}