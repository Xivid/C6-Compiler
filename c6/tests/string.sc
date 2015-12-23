//array of char
array str [10] = "66666666\n";
nasstr = "\hello\t\"world\"\n"; //the character \ in NAS is \ not \\
str[0] = 'a';
str[1] = 'b';
putc (str[0]);
puts ("This is a nas string: [%20s]", nasstr);
puts (str);
specialch = '\t';
putc(specialch);