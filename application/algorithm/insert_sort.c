void insert_sort(int * array, int n)
{
	int t = 0;
	for(int i = 0; i < n; i ++)
	{
		 t = array[i];
		for(j = i; j > 0 && x[j - 1] > t; j--)
		{
			array[j] = array[j - 1];
		}
		array[j] = t;
	}
}