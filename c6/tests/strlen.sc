//test pass real string by reference
length(&s[200]){
	l=0;
	while (s[l]!=0){
		l=l+1;
	}
	return l;
}

array g[100]="Good morning!\n";
puti(length(&g));

