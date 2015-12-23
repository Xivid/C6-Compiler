//test pass by reference 
change (&a){
	a = a+1;
	puti(a);
	return 1;
}

x =1;
change(&x);
puti(x);
