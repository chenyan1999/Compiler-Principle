int fibo(int a) {
    if(a == 1 || a == 2){
        return 2;
    }else{
        return fibo(a - 1) + fibo(a - 2);
    }
    return a;
}
int print(int a){
    
    return 0;
}
int main(){
	int m, n, i;
    m = 1;
    i = fibo(3);
    print(i);
    // i = fibo(3);
    // print(i);
	return 1;
}