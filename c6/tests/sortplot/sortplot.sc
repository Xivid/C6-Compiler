array points[100];
temp = 0;
for(x = 0; x < 100; x = x + 1;) {
	geti(temp);
	if (temp == -1) break;
	for(j = 0; points[j] < temp; j = j + 1;);
	for(k = x; k > j; k = k - 1;) points[k] = points[k-1];
	points[k] = temp;

	for (i = 0; i <= x; i = i + 1;) puti(points[i]);
	puti(-1);
}
