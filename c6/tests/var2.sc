//variables 2
seed = 13;
transform (a){
	if ((@seed/2)*2 == @seed){
		a = a*8;
	}
	else a = a+5;
	return a;
}

puti(transform(11));

