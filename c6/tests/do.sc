// FACTORIAL(x) with do loop

read x;
if (x == 0)
  print 1;
else
  if (x >= 1 && x <= 12) { // 13! is too much for int
    f = 1;
    do {
      f = f * x;
      x = x - 1;
    }
    while (x > 1) ;
    print f;
  }
