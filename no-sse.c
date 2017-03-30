// your C program without sse instructions

#include <stdio.h>

#include <stdlib.h>

#include <sys/time.h>

// Compile with: gcc -O2 -Wall float-triad-normal.c -o float-triad-normal -DN=.. -DR=..

// use -DN=.. and -DR=... at compilation
// defined here, compiled and run on Dev-C++
#define N 600	//image height (rows)
#define M 800	//image width (columns)


void get_walltime(double *wct) {
  struct timeval tp;
  gettimeofday(&tp,NULL);
  *wct = (double)(tp.tv_sec+tp.tv_usec/1000000.0);
}

//gennhtria tyxaiwn arithmwn gia tin arxikopoiisi tis eikonas
//phgh: http://www.insomnia.gr/topic/374643-%CF%84%CF%85%CF%87%CE%B1%CE%AF%CE%BF%CE%B9-%CE%B1%CF%81%CE%B9%CE%B8%CE%BC%CE%BF%CE%AF-%CF%83%CE%B5-c/
float _Random(int From, int To) {
	float Number;
	do
	{
		Number = rand() % (To + 1) ;
	}while(Number < From);
	
	return(Number);
}
	
int main() {
float *P,*K,*newP, sum;
int row,col;
int i,j;
double ts,te,mflops;


  // allocate test arrays
  P = (float *)malloc(N*M*sizeof(float)); // pinakas arxikis eikonas
  if (P==NULL) exit(1);
  K = (float *)malloc(3*4*sizeof(float)); //pinakas me statheres metasximatismou
  if (K==NULL) { free(P); exit(1); }
  newP = (float *)malloc(N*M*sizeof(float)); // pinakas telikis eikonas
  if (newP==NULL) { free(P); free(K); exit(1); }
 
  
  //initialize all arrays - cache warm-up
  for (i=0;i<N*M;i++) {
    	P[i]=_Random(-1000, 1000);	//arxikopoiisi eikonas me tyxaies times metaksy -1000 kai 1000
		newP[i]=0;						//arxikopoiisi-midenismos telikis eikonas
  }
  for (i=0;i<3*4;i++) {
  		if ((i+1)%4==1)		// an oxi stin 4i stili pou prostethike epipleon
		  K[i]=0.5;			//arxikopoiisi statherwn metasximatismou, me times opws dinodai kai epipleon 1 grammi kai stili gia na ginei 3x4
		else
			K[i]=0;			//arxikopoiisi 4is stilis me 0, ara den epireazei to apotelesma tou metasxhmatismou 
  }
  K[5]=5.0;				//kedriko simeio tou 3x3
  
  
  // get starting time (double, seconds) 
  get_walltime(&ts);
  
 
 	// do image tranform with 2 (row-col and i-j) loops on 1D arrays no pointers  						MFLOPS/sec = 382 gia 3x4, 374 gia 3x3
   for (row=1;row<N-1;row++) {  //gia kathe grammi xwris to perigramma tis eikonas
   	for (col=1;col<M-1;col++) { //gia kathe stili xwris to perigramma tis eikonas
	  	sum=0.0;
		for (i=0;i<3;i++) {
			for (j=0;j<4;j++) {
      			sum += P[(row+i-1)*M + col+j-1] * K[i*4 + j];
      		}
      	}
      	newP[row*M + col]=sum;
      }
  	}
  
  
  
  // get ending time
  get_walltime(&te);
  
  // compute mflops/sec (12 operations (3x4) per N-1 * M-1 passes - xwris to perigramma tis eikonas)
  mflops = ((N-1)*(M-1)*12*2.0)/((te-ts)*1e6);
  
  printf("MFLOPS/sec = %f\n",mflops);
  printf("Elapsed time: %f sec\n",(te-ts));
  
  // free arrays
  free(P); free(K); free(newP);
  
  return 0;
}
