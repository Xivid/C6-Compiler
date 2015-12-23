//tests functions
prime (x) {
	if (x==1) return 0;
	if (x==2) return 1;
	for (i=2; i<x; i=i+1;)
	{
	if ((x/i)*i == x) return 0;
	}
	return 1;
}

geti (x);
if (prime(x)==1) {
puts("This is a prime number!");
}
else {
puts("Nah,it's not a prime.");}


