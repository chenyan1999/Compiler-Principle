int fibo(int a) {
    int i, j;
    if(a == 1 || a == 2){
        return 1;
    }else{
        i = fibo(a - 1) + fibo(a - 2);
        return i;
    }
    return a;
}
int main(){
	int m, n, i;
    for (i = 1; i < 8; i ++){
        m = fibo(i);
        write(m);
    }
    // i = fibo(3);
    // print(i);
	return 1;
}