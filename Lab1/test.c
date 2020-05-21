int a, b, c;
float m, n;
char y;
struct ThisIsStruct{
    int num;
    float f;
    char c;
} astruct;
int X[5][10];
struct ThisIsStruct bstruct;
int fibo(int a) {
    astruct.num = 3;
    X[1][1] += 1;
	if (a == 1 || a == 2) return 1;
	return fibo(a--) + fibo(a-2);
    while(i > 0){
        print(i);
    }
}
int main(){
	int m, n, i;
	m = read();
	i = 1;
	n = ++ X[0][2];
    y = 'T';
	for(i = 1; i <= m; i ++){
		//Test this comment
		n = fibo(i);
		write(n);
        if (i = 4){
            break;
        }else{
            continue;
        }
	}
    /* while(i <= m){
         n = fibo(i);
	 	write(n);
    }*/
	return 1;
}

