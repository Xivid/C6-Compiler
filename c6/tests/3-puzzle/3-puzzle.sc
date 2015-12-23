array init[12];
init[0] = 1032;
init[1] = 132;
init[2] = 3102;
init[3] = 3120;
init[4] = 3021;
init[5] = 321;
init[6] = 2301;
init[7] = 2310;
init[8] = 2013;
init[9] = 213;
init[10] = 1203;

array best[12];
best[7] = 1230;
best[1] = 1032;



hash(state) {
	array fact[5];
	fact[0] = 1;
	fact[1] = 1;
	fact[2] = 2;
	fact[3] = 6;
	array digits[4];
	for (i = 3; i >= 0; i = i - 1;) {
		digits[i] = state % 10;
		state = state / 10;
	}
	ans = digits[0] * fact[3];
	for (i = 1; i < 4; i = i + 1;) {
		k = 0;
		for (j = 0; j < i; j = j + 1;)
			if (digits[j] < digits[i]) k = k + 1;
		ans = ans + (digits[i] - k) * fact[3-i];
	}
	return ans;
}

geti(x);
puti(hash(x));