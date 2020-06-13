//用于符号表测试
// int a, b, c;
// float m, n;
// int X[5][10];//array
// // int print(int a){
// //     int ax;
// //     return 1;
// // }
int fibo(int a) {
    int w,y[5][5], i;
    float cc[5][5][5],ff,aa;
    X[1][1] += 1; // +=
    a = 7+2;
	if (a == 9 || a == 10) return 11;//if else
	return fibo(a--) + fibo(a-12);//++
    while(i > 0){//while
        int w;
        print(i);
    }
}
// int main(){
// 	int m, n, i;
//     char y;
// 	m = read();
// 	i = 1;
// 	n = ++ X[0][2];
//     y = 'T';
// 	for(i = 1; i <= m; i ++){//for
// 		//Test this comment
// 		n = fibo(i);
// 		write(n);
//         if (i = 4){
//             break;//break
//         }else{
//             continue;//continue
//         }
// 	}
//     /* while(i <= m){
//          n = fibo(i);
// 	 	write(n);
//     }*/ 
// 	return 1;
// }

//以下用于静态语义检查
// int a, b, c;
// float m, n;
// int X[5][10];//array
// int print(int a){
//     int j;
//     char j;//变量重复定义
//     i ++; //使用未定义的变量
//     getchar();//调用未声明的函数
//     //函数没有返回值
// }
// int print(int a){//函数重复定义
//     int array[2][5][2];
//     float j;
//     write ++;//对函数名采用非函数调用
//     a();//对非函数名采用函数调用
//     write(a,b,c);//实参和形参数目不匹配
//     write(j);//实参和形参类型不匹配不匹配
//     j[2] = 3; //对非数组变量采用下标访问
//     array[2.5][2][9];//采用非整型变量做下标，不能识别数组越界或维数缺失
//     2 = 3;//赋值号左边不是赋值表达式
//     2 += 3;//自增自减运算仅限变量，否则无法识别
//     return 1.5;//返回值类型不匹配函数定义
// }
// int fibo(int a) {
//     int i;
//     X[1][1] += 1; // +=
// 	if (a == 1 || a == 2) return 1;//if else
// 	return fibo(a--) + fibo(a-2);//++
//     while(i > 0){//while
//         print(i);
//     }
// }
// int main(){
// 	int m, n, i;
//     char y;
// 	m = read();
// 	i = 1;
// 	n = ++ X[0][2];
//     y = 'T';
//     break;
//     continue;//break和continue不再循环语句中
// 	for(i = 1; i <= m; i ++){//for
// 		//Test this comment
// 		n = fibo(i);
// 		write(n);
//         if (i = 4){
//             break;//break
//         }else{
//             continue;//continue
//         }
// 	}
// 	return 1;
// }