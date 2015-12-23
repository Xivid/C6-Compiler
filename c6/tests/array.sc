puts("multiple dimensional array");
array a[10] = 8, arr[2,3,4], c[3], d[2,2,3] = 7;
geti(c[1], c[2]); // multiple geti
puti(c[1], c[2]); // multiple puti
arr[1,1,2] = d[1,1,2];
c[3] = a[1] + d[1,1,1];
array index[3];
index[0] = 9;
index[1] = 3;
index[2] = 1;
k = 1;
arr[k, k, index[2] + k] = 100;
puti(a[index[0]], a[d[1,1,1]], c, arr[index[2], index[2] , index[2] + k]);

puts("print integer array");
array nums[5] = 3;
puti(nums);

puts("array of nas strings");
array strs[3];
strs[0] = "\hello\t\"world\"\n"; //the character \ in NAS is \ not \\
gets(strs[1], strs[2]); // multiple gets
puts(strs[1], strs[2]); // multiple puts
puts ("This is a nas string: [%20s]\n", strs[0], strs[1], "literal"); // formatted puts

puts("real string array");
array str [10] = "66666666\n";
nasstr = "\hello\t\"world\"\n"; //the character \ in NAS is \ not \\
getc(str[0], str[1]); // multiple getc
putc (str[0], str[1]); // multiple putc
puts (str); // puts for real string