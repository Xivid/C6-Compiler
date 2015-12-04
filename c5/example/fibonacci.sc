// iterative calculation of fibonacci sequence
read n;

if ((n < 0) || (n > 40)) {
	;
} else {
	if (n == 0) {
		print 0;
	}
	if (n == 1) {
		print 1;
	}
	a = 0;
	b = 1;
	i = 2;
	s = 0;
	while (i <= n) {
		s = a + b;
		a = b;
		b = s;
		i = i + 1;
	}
	print s;
}
