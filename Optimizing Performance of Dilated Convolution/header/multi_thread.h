#include <pthread.h>

// Create other necessary functions here


// Structure to pass parameters to thread

typedef struct thread_arg{
    int row_start;
    int row_end;
    int *input;
    int input_row;
    int input_col;
    long long unsigned int *output;
    int output_row;
    int output_col;
    int *kernel;
    int kernel_row;
    int kernel_col;
}Arguments;


void *Convolution(void * arg){
    Arguments *param=(Arguments *)arg;
    int output_i,output_j,kernel_i;
    int input_j,output_row_offset,kernel_row_offset,input_row_offset,input_i;
    int input_j1,input_j2,input_j3,input_j4,input_j5,input_j6,input_j7,flag;
    int kernel_j,j,output_sum,output_sum1,output_sum2,output_sum3;
    int *C=NULL;
    
    __m256i a,b,c;

    int input_row=param->input_row;
    int input_col=param->input_col;
    int output_col=param->output_col;
    int kernel_row=param->kernel_row;
    int kernel_col=param->kernel_col;
    int *input=param->input;
    unsigned long long int *output=param->output;
    int *kernel=param->kernel;
    output_i=param->row_start;
    int output_row=param->row_end;
    output_row_offset=output_i*output_col;
    
    for(; output_i< output_row; output_i++)
    {
        for(output_j = 0; output_j< output_col-7; output_j+=8)
        {
            input_i=output_i;
            kernel_row_offset=0;
            int output_sum[16]={0};
            for(kernel_i = 0; kernel_i< kernel_row; kernel_i++)
            {
                input_row_offset=input_i*input_col;
                kernel_j=0;
                j=0;
                while(kernel_j!=kernel_col){
                    if(kernel_col-kernel_j>7){
                        j=kernel_j*2;
                        int column_offset[22];
                        int cntr=j;
                        for(int i=0;i<22;i++){
                            column_offset[i]=output_j+cntr++;
                            column_offset[i]=column_offset[i]%input_col;
                        }

                        for(int i=0;i<8;i++){
                            a=_mm256_set_epi32(input[column_offset[i+14]+input_row_offset],input[column_offset[i+12]+input_row_offset],
                                        input[column_offset[i+10]+input_row_offset],input[column_offset[i+8]+input_row_offset],
                                        input[column_offset[i+6]+input_row_offset],input[column_offset[i+4]+input_row_offset],
                                        input[column_offset[i+2]+input_row_offset],input[column_offset[i]+input_row_offset]);
                            b=_mm256_loadu_si256((__m256i *)&kernel[kernel_row_offset+kernel_j]);
                            c=_mm256_mullo_epi32(a,b);
                            C=(int *)(&c);
                            output_sum[i]+=C[0]+C[1]+C[2]+C[3]+C[4]+C[5]+C[6]+C[7];
                        }
                     
                        kernel_j+=8;
                    }
                    else{
                        input_j=(output_j+2*kernel_j);
                        input_j1=(input_j+1)%input_col;
                        input_j2=(input_j+2)%input_col;
                        input_j3=(input_j+3)%input_col;
                        input_j4=(input_j+4)%input_col;
                        input_j5=(input_j+5)%input_col;
                        input_j6=(input_j+6)%input_col;
                        input_j7=(input_j+7)%input_col;
                        input_j= (input_j)%input_col;
                        int kernel_val=kernel[kernel_row_offset+kernel_j];
                        output_sum[0]+= input[input_row_offset+input_j] * kernel_val; 
                        output_sum[1]+=input[input_row_offset+input_j1] * kernel_val;
                        output_sum[2]+=input[input_row_offset+input_j2] * kernel_val;
                        output_sum[3]+=input[input_row_offset+input_j3] * kernel_val;
                        output_sum[4]+=input[input_row_offset+input_j4] * kernel_val;
                        output_sum[5]+=input[input_row_offset+input_j5] * kernel_val;
                        output_sum[6]+=input[input_row_offset+input_j6] * kernel_val;
                        output_sum[7]+=input[input_row_offset+input_j7] * kernel_val;
                        kernel_j++;
                    }
                }     
                input_i=(input_i+2>=input_row)?input_i+2-input_row:input_i+2;
                kernel_row_offset+=kernel_col;
            }
            for(int i=0;i<8;i++){
                output[output_row_offset+output_j+i]=output_sum[i];
            }
        }

        while(output_j!=output_col){
            int output_sum[1]= {0};
            input_i=output_i;
            kernel_row_offset=0;

            for(int kernel_i = 0; kernel_i< kernel_row; kernel_i++)
            {
                input_row_offset=input_i*input_col;
                kernel_j=0;
                j=0;
                while(kernel_j!=kernel_col){
                    if(kernel_col-kernel_j>7){
                        j=kernel_j*2;

                        a=_mm256_set_epi32(input[(output_j+j+14)%input_col+input_row_offset],input[(output_j+j+12)%input_col+input_row_offset],
                                        input[(output_j+j+10)%input_col+input_row_offset],input[(output_j+j+8)%input_col+input_row_offset],
                                        input[(output_j+j+6)%input_col+input_row_offset],input[(output_j+j+4)%input_col+input_row_offset],
                                        input[(output_j+j+2)%input_col+input_row_offset],input[(output_j+j)%input_col+input_row_offset]);
                        
                        b=_mm256_loadu_si256((__m256i *)&kernel[kernel_row_offset+kernel_j]);
                        c=_mm256_mullo_epi32(a,b);
                        C=(int *)(&c);
                        output_sum[0]+=C[0]+C[1]+C[2]+C[3]+C[4]+C[5]+C[6]+C[7];
                        kernel_j+=8;
                    }
                    else{
                        input_j = (output_j+2*kernel_j) % input_col;
                        output_sum[0]+= input[input_row_offset+input_j] * kernel[kernel_row_offset +kernel_j]; 
                        kernel_j++;
                    }
                }
                input_i=(input_i+2>=input_row)?input_i+2-input_row:input_i+2;
                kernel_row_offset+=kernel_col;     
            }
            output[output_row_offset+output_j]=output_sum[0];
            output_j++;
        }
        output_row_offset+=output_col;

    }
    return NULL;
}


void multiThread( int input_row, 
                int input_col,
                int *input, 
                int kernel_row, 
                int kernel_col, 
                int *kernel,
                int output_row, 
                int output_col, 
                long long unsigned int *output ) 
{

    for(int i = 0; i < output_row * output_col; ++i)
            output[i] = 0;

    int no_of_threads=16;                               // set no of threads
    Arguments arg[no_of_threads];
    pthread_t thread[no_of_threads];
    int itr=output_row/no_of_threads;
    int count=0;
    int i;
    for(i=0;i<no_of_threads-1;i++){
        arg[i]={count,count+itr,input,input_row,input_col,output,output_row,output_col,kernel,kernel_row,kernel_col};
        count+=itr;
    }
    arg[i]={count,output_row,input,input_row,input_col,output,output_row,output_col,kernel,kernel_row,kernel_col};


    for(i=0;i<no_of_threads;i++)
        pthread_create(&thread[i],NULL,Convolution,(void *)&arg[i]);
   
    for(i=0;i<no_of_threads;i++){
        pthread_join(thread[i],NULL);
    }

}