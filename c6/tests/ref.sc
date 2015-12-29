//test pass by reference 
change (&a){
	a = a+1;
	puti(a);
	return 1;
}

x =1;
change(&x);
puti(x);

//test reference of arrays
alter(&a[10]){
	a[0] =5;
	return 1;
}

sum(&a[10]){
	s=0;
	for (i=0;i<10;i=i+1;)
	{
		s= s+a[i];
		puti(a[i]);
	}
	return s;
}

array nums[10] = 3;
alter(&nums);
puti(sum(&nums));
puti(nums);

//test pass real string by reference
length(&s[200]){
	l=0;
	while (s[l]!=0){
		l=l+1;
	}
	return l;
}

array g[100]="Good morning!";
puts_("The length of [");
puts_(g);
puts_("] is: ");
puti(length(&g));