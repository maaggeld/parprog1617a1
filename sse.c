// your C program using sse instructions
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <emmintrin.h>

// Compile with: gcc -msse2 -O2 -Wall float-triad-sse.c -o float-triad-sse -DN=.. -DR=..


// use -DN=.. and -DR=... at compilation
// defined here, compiled and run on Dev-C++
#define N 600	//image height (rows)
#define M 800	//image width (columns)


#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
//enallaktika ths posix_memalign logw windows ousiastika xrisimopoiw thn isodynami _aligned_malloc pou apla sydasetai ligo diaforetika
// phgh: http://stackoverflow.com/questions/33696092/whats-the-correct-replacement-for-posix-memalign-in-windows
// free memory has also to change, see at the end.

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
float *P,*K,*newP, *sum;
__m128 *va,*vb,*vc, *vsum;

int row,col;
int i,j;
double ts,te,mflops;

  // allocate test arrays - request alignment at 16 bytes
  i = posix_memalign((void **)&P,16,N*M*sizeof(float));  // pinakas arxikis eikonas
  if (i!=0) exit(1);
  i = posix_memalign((void **)&K,16,3*4*sizeof(float)); //pinakas me statheres metasximatismou
  if (i!=0) { free(P); exit(1); }
  i = posix_memalign((void **)&newP,16,N*M*sizeof(float));  // pinakas telikis eikonas
  if (i!=0) { free(P); free(K); exit(1); }
  i = posix_memalign((void **)&sum,16,4*sizeof(float));  // metavliti athroismatos
  if (i!=0) { free(P); free(K); free(newP); exit(1); }

  
   //initialize all arrays - cache warm-up
  for (i=0;i<N*M;i++) {
    	P[i]=_Random(-1000, 1000);	//arxikopoiisi eikonas me tyxaies times metaksy -1000 kai 1000
		newP[i]=0;						//arxikopoiisi-midenismos telikis eikonas
  }
  for (i=0;i<3*4;i++) {
  	if ((i+1)%4==1)
		K[i]=0.5;			//arxikopoiisi statherwn metasximatismou, me times opws dinodai kai epipleon 1 grammi kai stili gia na ginei 3x4
	else
		K[i]=0; 
  }
  K[5]=5.0;				//kedriko simeio toy 3x3
  
  for (i=0;i<4;i++) {
	sum[i]=0.0;			//arxikopoiisi athroismatos
  }
 
  // get starting time (double, seconds) 
  get_walltime(&ts);
  	
  

  for (row=1;row<N-1;row++) {  //gia kathe grammi xwris to perigramma tis eikonas												MFLOPS/sec = 716
   	for (col=1;col<M-1;col++) { //gia kathe stili xwris to perigramma tis eikonas
	  	
	  	// alias the sse pointers to output array and sum variable
	  	vc = (__m128 *)(newP+(row*M) + (col%4)*4);			// (col modulo 4)*4 gia na exoume pada pollaplasio tou 4, ara euthygrammismeno sta 16 bytes
		vsum = (__m128 *)sum;
		for (i=0;i<3;i++) {
			// alias the sse pointers to arrays
			va = (__m128 *)(P+ (row+i-1)*M + ((col-1)%4)*4 );
			vb = (__m128 *)(K+ i*4);
    	
		//	for (j=0;j<4;j++) {
      			*vsum =  _mm_add_ps(*vsum, _mm_mul_ps(*va,*vb));
				//sum += P[(row+i-1)*M + col+j-1] * K[i*4 + j];
      	//	}
      	}
	    *vc=*vsum;
		//newP[row*M + col]=sum;
   }
  }
  
  // get ending time
  get_walltime(&te);
  
  // compute mflops/sec (12 operations per N*M passes)
  mflops = ((N-1)*(M-1)*12*2.0)/((te-ts)*1e6);
  
  printf("MFLOPS/sec = %f\n",mflops);
  printf("Elapsed time: %f sec\n",(te-ts));
  
  // free arrays
 // free(a); free(b); free(c); free(d);
 _aligned_free(P); _aligned_free(K); _aligned_free(newP); //because of using _aligned_malloc (in windows) instead of posix_memalign (in linux)
  
  return 0;
}
