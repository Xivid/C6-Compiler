a = 0;
b = 0;

do {
    a = a + 1;
    for (i = 0; i < 10; i = i + 1;){
        b = b + 1;
    }
} while ( a != 256 );

print a;
print b;

b = 0;

do {
    a = a + 1;

    for(i = 0; i < 10; i = i + 1;){
        if(i % 3){
            continue;
        }

        for(j = 0; j < 5; j = j + 1;){
            if(j % 2){
                continue;
            }

            if(j == 4){
                break;
            }

            b = b + 2;
        }
    }

    if (a == 510) {
        break;
    }
} while (a != 1024);

print a;
print b;
