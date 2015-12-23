#include <stdio.h>
int main() {
	int i = 0;
	int temp;
	int arr[100], t = 0;
	while (scanf("%d", &temp) != EOF) {
		if (temp == -1) {
			for(i=0; i<t; i++) printf("%d ", arr[i]);
			printf("\n");
			t = 0;
		} else arr[t++] = temp;
	}
	return 0;
}
