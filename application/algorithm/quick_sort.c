void swap(int&a, int&b)
{
	int c = a;
	a = b;
	b = c;
}
void qsort(int * array, int l, int r)
{
	int m = l;
	for(int i = l + 1; i < r; i++)
	{
		if( array[i] > array[m])
		   swap(array[++m], array[i]); 
	}
	swap(array[l], array[m]);
	qsort(array, l, m - 1);
	qsort(array, m + 1, r);
}

void qsort1(int *array, int l, int r)
{
	while(true)
	{
		for(int j = l + 1; j < r && array[j] < array[l]; j++) ;
		for(int k = u; k > l && array[k] > array[l]; k--);
		if( j > k)
		 break;
		 swap(array[j], array[k]);
	}
	swap(array[l],array[j]);
	qsort1(array, l, j - 1);
	qsort1(array, j + 1, r);
	
}