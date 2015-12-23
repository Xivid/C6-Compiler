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
