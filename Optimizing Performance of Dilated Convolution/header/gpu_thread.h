
/////////////////////////////////  Without Constant Memory  //////////////////////////////////////////////////////////


__global__ void mul(int * input ,int irow,int icol,int * d_kernel,int krow,int kcol, long long unsigned int * output,int rowlen , int collen){

        int ROW = blockIdx.y*blockDim.y+threadIdx.y;
    int COL = blockIdx.x*blockDim.x+threadIdx.x;
         if (ROW < rowlen && COL < collen) {


            int sum = 0;
            for(int kernel_i = 0; kernel_i< krow; kernel_i++)
            {
                for(int kernel_j = 0; kernel_j< kcol; kernel_j++)
                {
                    int input_i = (ROW + 2*kernel_i) % irow;
                    int input_j = (COL + 2*kernel_j) % icol;
                    sum += input[input_i*icol +input_j]* d_kernel[kernel_i*kcol +kernel_j];
                }
            }
            output[ROW*collen + COL] = sum;

    }
}
void gpuThread( int input_row, 
                int input_col,
                int *input, 
                int kernel_row, 
                int kernel_col, 
                int *kernel,
                int output_row, 
                int output_col, 
                long long unsigned int *output ) 
{
    size_t in = input_col*input_row*sizeof(int);
    size_t ker = kernel_col*kernel_row*sizeof(int);
    size_t op = output_col*output_row*sizeof(long long unsigned int);
   int * d_input,*d_kernel;

    long long unsigned int * d_output;
    cudaMalloc((void**)&d_input, in);
    cudaMalloc((void**)&d_output,op);
    cudaMalloc((void**)&d_kernel,ker);
    cudaMemcpy(d_input,input,in,cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel,kernel,ker,cudaMemcpyHostToDevice);  

    dim3 dimBlock(32,24,1);
    dim3 dimGrid(ceil(output_col/32.0),ceil(output_row/24.0));
    mul<<<dimGrid,dimBlock>>>(d_input,input_row,input_col,d_kernel,kernel_row, kernel_col, d_output,output_row,output_col);

    cudaMemcpy(output,d_output,output_col*output_row*sizeof(long long unsigned int),cudaMemcpyDeviceToHost);
}


///////////////////////// With Constant Memory /////////////////////////////////////////////////////////////////////


// __constant__ int d_kernel[16384];

// __global__ void mul(int * input ,int irow,int icol,int krow,int kcol, long long unsigned int * output,int rowlen , int collen){

//         int ROW = blockIdx.y*blockDim.y+threadIdx.y;
//     int COL = blockIdx.x*blockDim.x+threadIdx.x;
//          if (ROW < rowlen && COL < collen) {


//             int sum = 0;
//             for(int kernel_i = 0; kernel_i< krow; kernel_i++)
//             {
//                 for(int kernel_j = 0; kernel_j< kcol; kernel_j++)
//                 {
//                     int input_i = (ROW + 2*kernel_i) % irow;
//                     int input_j = (COL + 2*kernel_j) % icol;
//                     sum += input[input_i*icol +input_j]* d_kernel[kernel_i*kcol +kernel_j];
//                 }
//             }
//             output[ROW*collen + COL] = sum;

//     }
// }





// void gpuThread( int input_row, 
//                 int input_col,
//                 int *input, 
//                 int kernel_row, 
//                 int kernel_col, 
//                 int *kernel,
//                 int output_row, 
//                 int output_col, 
//                 long long unsigned int *output ) 
// {
//     size_t in = input_col*input_row*sizeof(int);
//     size_t ker = kernel_col*kernel_row*sizeof(int);
//     size_t op = output_col*output_row*sizeof(long long unsigned int);
//    int * d_input;

//     long long unsigned int * d_output;
//     cudaMalloc((void**)&d_input, in);
//     cudaMalloc((void**)&d_output,op);
//     cudaMemcpy(d_input,input,in,cudaMemcpyHostToDevice);
//     cudaMemcpyToSymbol(d_kernel,kernel,ker,0,cudaMemcpyHostToDevice);
    

//     dim3 dimBlock(32,24,1);
//     dim3 dimGrid(ceil(output_col/32.0),ceil(output_row/24.0));
//    mul<<<dimGrid,dimBlock>>>(d_input,input_row,input_col,kernel_row,kernel_col,d_output,output_row,output_col); 
//     cudaMemcpy(output,d_output,output_col*output_row*sizeof(long long unsigned int),cudaMemcpyDeviceToHost);
// }