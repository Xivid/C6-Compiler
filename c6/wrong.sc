array a[4]; a[0] = 1; a[1] = 0; a[2] = 0; a[3] = 1; ar = 2; ac = 2; array b[4]; b[0] = 1; b[1] = 2; b[2] = 3; b[3] = 4; br = 2; bc = 2; array product[4]; for(i = 0; i < 4; i = i+1;) product[i] = 0; bcols = 0; brows = 0; arows = 0; for(brows = 0; brows < br; brows = brows + 1;)	{ for(bcols = 0; bcols < bc; bcols = bcols + 1;) { con = b[brows*bc+bcols]; if(con != 0) { for(arows = 0; arows < ar; arows = arows + 1;) { product[ arows*bc+bcols] = product[ arows*bc+bcols] + a[arows*ac + brows] * con; } } } } puti(product);
