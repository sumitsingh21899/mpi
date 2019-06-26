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
    float update(int myoffset, int chunk, int myid);
    MPI_Status status;
    struct timeval in,f;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    numtasks=tasks-1;
    
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
//    printf ("MPI task %d has started...\n", taskid);
   
    tag2 = 1;
    tag1 = 2;

    if (taskid == 0){
      max= 0;
     for(i=0; i<ARRAYSIZE; i++) {
            data[i] =  i;
            max=(max>data[i])?max:data[i];
            }
  //  printf("Initialized array max = %e\n",max);
    gettimeofday(&in,NULL);
        offset=0;
    for (dest=1; dest<tasks; dest++) {
        chunksize=(dest*ARRAYSIZE)/numtasks - ((dest-1)*(ARRAYSIZE))/numtasks;
            MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            MPI_Send(&data[offset], chunksize, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD);
    //        printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
            offset = offset + chunksize;
    }
 mymax=0;
    MPI_Reduce(&mymax, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    
   // printf("Sample results: \n");
    offset = 0;
   // printf("*** Final max= %e ***\n",max);
    gettimeofday(&f,NULL);
   // float elapsed = (f.tv_sec*1000000 + f.tv_usec) -
     //           (in.tv_sec*1000000 + in.tv_usec);
float elapsed = (f.tv_sec) -
                (in.tv_sec);
     
//printf("Elasped time= %f Seconds \n\n",(1.0*elapsed)/1000000);
printf("Elapsed time  = %f seconds \n",elapsed);
  } 
    if (taskid > 0) {
          source = 0;
        chunksize=(taskid*ARRAYSIZE)/numtasks - ((taskid-1)*(ARRAYSIZE))/numtasks;
          MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
          MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2, 
            MPI_COMM_WORLD, &status);

          mymax = update(offset, chunksize, taskid);
        max=(max>mymax)?max:mymax;
          MPI_Reduce(&mymax, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
  } 
    MPI_Finalize();
}   
float update(int myoffset, int chunk, int myid) {
    int i; 
    float mymax=0;

    for(i=myoffset; i < myoffset + chunk; i++) {
            data[i] =  i ;
            mymax=(mymax>data[i])?mymax:data[i];
    }
  //  printf("Task %d mymax = %e\n",myid,mymax);
    return(mymax);
}
