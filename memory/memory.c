











/////////////////////////////////////////////////////////////////////////////////
// BÀI 06 - TỰ CÀI ĐẶT STRING LENGTH

// Đề bài:
// Viết StringLength() để tính độ dài chuỗi C mà không dùng strlen().
// Không tính ký tự kết thúc chuỗi '\0'.
// Hàm trả về false nếu tham số không hợp lệ.


// #include <stdbool.h>
// #include <stddef.h>
// #include <stdio.h>

// bool StringLength(const char *text, size_t *length)
// {
//     if (text == NULL || length == NULL){
//         return false ;
//     }

//     size_t count = 0 ;

//     while (!(text[count] == '\0')){

//         count ++ ;
//     }


//     *length = count ;
//     return true ;


// }

// int main(void)
// {
//     const char text[] = "Embedded C";
//     size_t length = 0U;

//     if (StringLength(text, &length))
//     {
//         printf("Do dai: %zu\n", length);
//     }

//     return 0;
// }


/////////////////////////////////////////////////////////////////////////////////

//  * BAI 3: TIM SO LON THU HAI TRONG MANG
//  *
//  * DE BAI:
//  * Tim gia tri lon thu hai khac gia tri lon nhat.
//  * Khong sap xep toan bo mang.
//  * Vi du: {5, 2, 9, 7, 9, 3} -> 7.

// #include <stdio.h>
// #include <stdint.h>
// #include <stdbool.h>
// #include <stddef.h>
// #include <limits.h>
// #include <ctype.h>


// static bool find_second(int *arry,int size, int *resurt ){

//     bool has_lastest = false ;
//     bool hase_second = false ;
//     int lastest = 0 ;
//     int second = 0 ;

//     for(int i = 0 ; i < size ; i++){
//         int value = arry[i] ;
//         if ( !has_lastest || value > lastest ){
            
//             if( has_lastest ){

//                 second = lastest ;
//                 hase_second = true ;

//             }

//             lastest = value ;
//             has_lastest = true ;

//         }
//         else if ( (value < lastest ) && (value >second || !hase_second  ) ){
//             second = value ;
//             hase_second = true ;
//         }

//     }

//     *resurt = second  ;

//     return true ;

// }

// int main(void){
//     int array [] = {23,64,43,2,3,2,89,1,2,3,32,34};
//     int resurt = 0 ;
//     int size = sizeof(array) / sizeof(array[0]) ;
//     if (find_second(array, size ,&resurt  )){
    
//         printf(" resurt %d \n ",resurt) ;

//     }else{
//         printf(" no resurt ") ;

//     }
// }



/////////////////////////////////////////////////////////////////////////////////




// #include <stddef.h>
// #include <stdint.h>
// #include <stdio.h>

// int CompareBuffer(const void *buffer1, const void *buffer2, size_t length)
// {
//     if (length == 0U)
//     {
//         return 0;
//     }

//     if (buffer1 == NULL || buffer2 == NULL)
//     {
//         return (buffer1 == buffer2) ? 0 : ((buffer1 == NULL) ? -1 : 1);
//     }

//     const uint8_t *a = (const uint8_t *)buffer1;
//     const uint8_t *b = (const uint8_t *)buffer2;

//     for (size_t i = 0U; i < length; ++i)
//     {
//         if (a[i] < b[i])
//         {
//             return -1;
//         }
//         if (a[i] > b[i])
//         {
//             return 1;
//         }
//     }

//     return 0;
// }

// int main(void)
// {
//     const uint8_t a[] = {1U, 2U, 3U, 4U};
//     const uint8_t b[] = {1U, 2U, 5U, 4U};

//     printf("Ket qua so sanh: %d\n", CompareBuffer(a, b, sizeof(a)));
//     return 0;
// }












/////////////////////////////////////////////////////////////////////////////////


// BÀI 03 - TỰ CÀI ĐẶT MEMORY SET

// Đề bài:
// Viết FillBuffer() để gán cùng một giá trị byte cho toàn bộ vùng nhớ,
// không sử dụng memset().

// Ví dụ:
//     FillBuffer(buffer, 0xAA, 8)
//     buffer = AA AA AA AA AA AA AA AA


/////////////////////////////////////////////////////////////////////////////////
// #include <stdbool.h>
// #include <stddef.h>
// #include <stdint.h>
// #include <stdio.h>

// bool FillBuffer(void *buffer, uint8_t value, size_t length){
//     if (buffer == NULL && length != 0U ){
//         return false ;
//     }
    
//     uint8_t *buyte = (uint8_t *)buffer ;
//     for (size_t i = 0 ; i<length; i++){
//         buyte[i] = value;
//     }
//     return true ;
// }

// int main(void) {
//     uint8_t buffer[10] ;
//     (void) FillBuffer(buffer,0xAAU, sizeof(buffer)) ;
//     for (size_t i = 0U ; i < sizeof(buffer); i++){
//         printf ("%02X \n",buffer[i]);
//     }
//     return 0;
// }


// BÀI 02 - COPY BUFFER CÓ XỬ LÝ CHỒNG LẤN

// Đề bài:
// Viết MoveBuffer() tương tự memmove(), nhưng không dùng memcpy() hoặc memmove().
// Nếu vùng đích nằm phía sau và chồng lên vùng nguồn, phải sao chép từ cuối về đầu.
// Nếu không, sao chép từ đầu đến cuối.

// Ví dụ:
//     buffer ban đầu: 0 1 2 3 4 5 6 7 8 9
//     MoveBuffer(&buffer[2], &buffer[0], 8)
//     kết quả:        0 1 0 1 2 3 4 5 6 7


/////////////////////////////////////////////////////////////////////////////////
// #include <stdbool.h>
// #include <stddef.h>
// #include <stdint.h>
// #include <stdio.h>
// bool MoveBuffer(void *destination, const void *source, size_t length ){

//     if ((destination == NULL || source == NULL) && length != 0U ){

//         return false ;
//     }

//     uint8_t *dest = (uint8_t *)destination ;
//     const uint8_t *src  = (const uint8_t *)source ;

//     if (dest == src || length == 0) {
//         return true ;
//     }

//     const uintptr_t dst_address = (uintptr_t) dest ;
//     const uintptr_t src_address = (uintptr_t) src ;

//     if ((dst_address > src_address) && (dst_address - src_address) < length ){
//         for (size_t i = length ; i > 0 ; i--){
//             dest[i - 1U] = src[i - 1U] ;
//         }

//     }else{
//             for(size_t i = 0; i < length ; i++ ){
//             dest[i] = src[i] ;
//         }
//     }

//     return true ;
// }
// int main(void){
//     uint8_t buffer[10] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U};

//     (void)MoveBuffer(&buffer[2], &buffer[0], 8U);

//     for (size_t i = 0U; i < sizeof(buffer); ++i)
//     {
//         printf("%u ", buffer[i]);
//     }
//     putchar('\n');

//     return 0;
// }


/* ================================================================
 * BAI 1: DAO NGUOC SO NGUYEN
 *
 * DE BAI:
 * Viet ham dao nguoc mot so nguyen ma khong chuyen so thanh chuoi.
 * Vi du: 12345 -> 54321.
 * Ham can phat hien truong hop ket qua bi tran kieu int.
 * ================================================================ */

// #include <stdio.h>
// #include <stdbool.h>
// int reverst_int (int number){
//     int out_put = 0;
//     if (number == 0){
//         return false ;
        
//     }
//     while (number != 0){
//         out_put = out_put*10 + number%10 ;
//         number = number /10;
//     }
//     return out_put ;
    
// }
// int main(void){
//     int a = 12345234 ;
//     int out = reverst_int(a);
//     printf("out: %d \n",out);
//     return 0;
    
// }
////////////////////////////////////////////////////////////////////////////////////


/* ================================================================
 * BAI 2: KIEM TRA SO NGUYEN TO
 *
 * DE BAI:
 * Viet ham tra ve true neu n la so nguyen to, nguoc lai tra ve false.
 * Chi can kiem tra cac uoc tu 2 den can bac hai cua n.
 * ================================================================ */
 
//  #include <stdio.h>
 
//  int arry [10000] ;
//  void sieve(void){
//      for (int i=0;i<=10000;i++){
//          arry[i] = 1 ;
//      }
   
//      arry[0] = arry[1] = 0 ;
//      for(int i=2 ;i<10000;i++){
//          for (int j = i*i ; j<=10000; j +=i){
//              arry [j] = 0 ;
             
//          }
         
//      }
     
//  }
 
//  int main(void){
//     int values[] = {2, 17, 20, 97, 1, -5};
//     int size = sizeof (values) / sizeof(values[0]) ;
    
//     sieve();
    
//     for (int i = 0 ; i<= size; i ++){
//         if (arry[values[i]] == 1 ){
//             printf ( "so nguyen to %d \n",values[i] )  ;
//         }
        
//     }
    
//      return 0 ;
     
     
     
//  }




////////////////////////////////////////////////////////////////////////////////////

/*
BÀI 02 - COPY BUFFER CÓ XỬ LÝ CHỒNG LẤN

Đề bài:
Viết MoveBuffer() tương tự memmove(), nhưng không dùng memcpy() hoặc memmove().
Nếu vùng đích nằm phía sau và chồng lên vùng nguồn, phải sao chép từ cuối về đầu.
Nếu không, sao chép từ đầu đến cuối.

Ví dụ:
    buffer ban đầu: 0 1 2 3 4 5 6 7 8 9
    MoveBuffer(&buffer[2], &buffer[0], 8)
    kết quả:        0 1 0 1 2 3 4 5 6 7
*/
////////////////////////////////////////////////

















