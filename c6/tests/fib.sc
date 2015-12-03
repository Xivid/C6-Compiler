//FIB (x)
//prints out the fibonacci serial 0,1,1,2,3,5,8..
read x;

i=0;
j=1;
if (x==0) print 0;
else {
		print 0;
		print 1;
		while (x>1){
			if (i<j) {i = i+j; print i;}
			else {j= i+j;print j;}
			x= x-1;
			}	
	}
}
