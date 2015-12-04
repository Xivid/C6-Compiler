a=0;

do {
    a = a + 1; 
} while ( a <= 256 );

print a;

do {
    a = a + 1;

    if (a == 510) {
        break;
    }
} while (a <= 1024);

print a;
