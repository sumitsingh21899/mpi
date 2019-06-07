#include <mpi.h>
#include<sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#define  ARRAYSIZE    100000000

float  data[ARRAYSIZE];

int main (int argc, char *argv[])
{
    int   numtasks,tasks, taskid, rc, dest, offset, i, j, tag1, tag2, source, chunksize; 
    float mymax, max;
    float update(int myoffset, int chunk, int myid);//calculates max for each task 
    MPI_Status status;
    struct timeval in,f;
/***** Initializations *****/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    numtasks=tasks-1;
    if ((numtasks) % 2 != 0) {
           printf("Quitting. Number of MPI tasks must be divisible by 2.\n");
    // MPI_Abort(MPI_COMM_WORLD, rc);
       exit(0);
       }
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    printf ("MPI task %d has started...\n", taskid);
    chunksize = (ARRAYSIZE / numtasks);
    tag2 = 1;
    tag1 = 2;
/***** Master task only ******/
    if (taskid == 0){
    
 /* Initialize the array */
      max= 0;
     for(i=0; i<ARRAYSIZE; i++) {
            data[i] =  i;
            max=(max>data[i])?max:data[i];
            }
    printf("Initialized array max = %e\n",max);
    gettimeofday(&in,NULL);
/* Send each task its portion of the array - master keeps 1st part */
    //offset = chunksize;
    offset=0;
    for (dest=1; dest<tasks; dest++) {
            MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            MPI_Send(&data[offset], chunksize, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD);
            printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
            offset = offset + chunksize;
    }

/* Master does its part of the work */
 mymax=0;

 

/* Get final max and print sample results */  
    MPI_Reduce(&mymax, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    
    printf("Sample results: \n");
    offset = 0;
    printf("*** Final max= %e ***\n",max);
    gettimeofday(&f,NULL);
    int elapsed = (f.tv_sec*1000000 + f.tv_usec) -
                (in.tv_sec*1000000 + in.tv_usec);
printf("Elasped time= %d microseconds \n\n",elapsed);

  } 
/* end of master section */



/***** Non-master tasks only *****/

    if (taskid > 0) {

/* Receive my portion of array from the master task */
          source = 0;
          MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
          MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2, 
            MPI_COMM_WORLD, &status);

          mymax = update(offset, chunksize, taskid);
        max=(max>mymax)?max:mymax;
          MPI_Reduce(&mymax, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

  } 
/* end of non-master */
    
    MPI_Finalize();

}   
/* end of main */


float update(int myoffset, int chunk, int myid) {
    int i; 
    float mymax=0;
/* compare  each of my array elements and keep my max */
    for(i=myoffset; i < myoffset + chunk; i++) {
            data[i] =  i ;
            mymax=(mymax>data[i])?mymax:data[i];
    }
    printf("Task %d mymax = %e\n",myid,mymax);
    return(mymax);
  }
