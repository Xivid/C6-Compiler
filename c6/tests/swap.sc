//sequence of params
swap(&a,&b){
	c = a;
	a = b;
	b = c;
	return 1;
}

x=10;
y=1;
puti(y);
swap(&x,&y);
puti(y);
