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

array best[22];
best[7] = 1230;
best[1] = 1032;
best[20] = 132;
best[21] = 3102;
best[19] = 3120;
best[5] = 2301;
best[16] = 2310;
best[17] = 2013;
best[12] = 213;
best[2] = 1203;
best[8] = 1230;

next(state, c) {
	if (c < '1' || c > '3') return state;
	array board[4];
	space = 0;
	chosen = 0;
	temp = state;
	for (i = 3; i >= 0; i = i - 1;) {
		board[i] = temp % 10;
		if (board[i] == 0)
			space = i;
		else if (board[i] == c - '0')
			chosen = i;
		temp = temp / 10;
	}
	if (space == 0) { 
		if (chosen == 1) 
			state = (board[1] * 100 + board[2]) * 10 + board[3];
		else if (chosen == 2)
			state = ((board[2] * 10 + board[1]) * 10) * 10 + board[3];
	} else if (space == 1) {
		if (chosen == 0)
			state = (board[0] * 10 + board[2]) * 10 + board[3];
		else if (chosen == 3)
			state = ((board[0] * 10 + board[3]) * 10 + board[2]) * 10;
	} else if (space == 2) {
		if (chosen == 0)
			state = (board[1] * 10 + board[0]) * 10 + board[3];
		else if (chosen == 3)
			state = ((board[0] * 10 + board[1]) * 10 + board[3]) * 10;
	} else { // space == 3
		if (chosen == 1)
			state = (board[0] * 100 + board[2]) * 10 + board[1];
		else if (chosen == 2)
			state = (board[0] * 10 + board[1]) * 100 + board[2];
	}
	return state;
}

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

puts("BEGIN");
geti(lucky);
cur = init[lucky % 11];
step = 0;
puti("DRAW %d\n", cur);
puts("STEP 0");
while (cur != 1230) {
	getc(c);
	if (c == '\n' || c == '\t' || c == ' ')
		continue;
	else if (c == '?') {
		cur = best[hash(cur)];
	}
	else if (c == 'E')
		break;
	else if (c == 'A') {
		puts("BEGIN");
		geti(lucky);
		cur = init[lucky % 11];
		step = 0;
		puti("DRAW %d\n", cur);
		puts("STEP 0");
		continue;
	} else {
		tmp = next(cur, c);
		if (tmp != cur)
			cur = tmp;
		else {
			puts("ILLEGAL");
			continue;
		}
	}
	step = step + 1;
	puts("DRAW %d\n", cur);
	puts("STEP %d\n", step);
	if (cur == 1230) {
		puts("SUCCESS");
		getc(c);
		while (c == '\n' || c == '\t' || c == '') 
			getc(c);
		if (c == 'A') {
			puts("BEGIN");
			geti(lucky);
			cur = init[lucky % 11];
			step = 0;
			puti("DRAW %d\n", cur);
			puts("STEP 0");
		}
	}
}
puts("END");