fun(x) {
	puti(123);
	return 1;
}

fun2(y) {
	puts("call fun");
	fun(2);
	puts("fun2");
	return 2;
}

puts("hello");
fun(333);
puts("world");

