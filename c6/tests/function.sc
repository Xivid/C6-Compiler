//Prime
prime (x) {
	if (x<=1) return 0;
	if (x==2) return 1;
	for (i=2; i<x; i=i+1;)
	{
	if ((x/i)*i == x) return 0;
	}
	return 1;
}

puts_("Prime - Input a integer: ");
geti (x);
puti_(x);
if (prime (x))
	puts(" is a prime");
else
	puts(" is not a prime");


//MAX
max (x, y){
	if (x> y) z=x;
	else z=y;
	return z;
}

puts_("MAX - Input two integers: ");
geti(a,b);
puti (a,b,max(a,b),max(a,2));

//Fibonacci

fib(x) {
	if (x == 0 || x == 1) return 1;
	return fib(x-1) + fib(x-2);
}

puts("Fibonacci - Please input a number: ");
geti(x);
puti(fib(x));