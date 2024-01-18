For Single Thread :-
	1. single_thread.h contains two functions :-
		a) singleThread :- It contains code for final optimization combining both optimization 1 and 2 together.
		b) singleThread_optimization_1 :- This function contains code for optimization 1 only.
		
For Multi Thread:-
	1. multi_thread.h  contains only one function :-
		a) multiThread :-It contains implementation for multi threaded DC. 
				 One can change no of threads by changing value of parameter no_of_threads inside multiThread function.
				 

For GPU/CUDA implementation :-
	1. gpu_thread.h contains below function:-
		a) gpuThread :-
			We have implimented it in 32x24 thread block size we can run it in different size.
			## For Changing Thread Block Size

			``````
			dim3 dimBlock(32,24,1);
			dim3 dimGrid(ceil(output_col/32.0),ceil(output_row/24.0));
			``````
			Here dimblock defines 32x24 size thread block.If we want to change the thread block size to 16x16 then we have to apply change as follows.

			``````
			dim3 dimBlock(16,16,1);
			dim3 dimGrid(ceil(output_col/16.0),ceil(output_row/16.0));
	``````
					 

