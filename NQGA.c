/*
 * Nicholas Marasco
 * CIS 421 - Artificial Intelligence
 * Assingment 2
 * Due Wed, Oct 5 10PM
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "indv.h"

// define global constants
#define N 12
#define MAX_GENS 1000
#define MUTATION 10

// globals
int solnFound = 0;
int popSize = N * 10;
int parentNum = N % 2 == 0 ? N : N + 1;

// GA functions
indv** initializePopulation();
void evalFitness(indv**);
float getFitValue(int*);
indv** dukeItOut(indv**);
indv** breed(indv**);
void survival(indv**,indv**);

// utility functions
void swap(int*,int,int);
void shuffle(int*);
int contains(int*,int,int);
int cmpIndv(const void*,const void*);
void destroyPop(indv**,int);
void killStragglers(indv**,int);
int getRand(int max);

// debug/print functions
void printPools(indv**,int);
void printFit(indv**,int);
void printSol(char*,int*);

int main(int ARGC, char *ARGV[]){
  srand(time(0));
  int currGens = 0;

  indv **pop = initializePopulation();
  indv **parents;
  indv **children;
//   printPools(pop,popSize);
  evalFitness(pop);
//   printFit(pop,N*10);

  while(!solnFound && currGens < MAX_GENS){
    parents = dukeItOut(pop);
//     printf("\nparents:");
//     printPools(parents,parentNum);

    children = breed(parents);
//     printf("\nchildren:");
//     printPools(children,parentNum);

    survival(pop,children);
//     printf("\nnew pop:");
//     printPools(pop,popSize);
//     printFit(pop,popSize);

    currGens++;
  }
  qsort(pop,popSize,sizeof(indv*),cmpIndv);

  FILE *fout = fopen("output.txt","a");

//   printf("Gens: %d\n",currGens);
  fprintf(fout,"Gens: %d\n",currGens);
  if(currGens != MAX_GENS){
//     printSol("Sol:",pop[0]->solution);
    int i;
    fprintf(fout,"Sol:");
    for(i = 0; i < N; i++){
      fprintf(fout," %d",pop[0]->solution[i]);
    }
  }
  fprintf(fout,"\n\n");

//   printf("Fit: %f\n",pop[0]->fitness);
  printf("hep\n");
  destroyPop(pop,popSize);
  if(currGens != 0){
    free(parents);
    free(children);
  }
}

// select the new generations population
// parameters:
//  pop - the population to be updated
//  children - the newly bred children
void survival(indv **pop, indv **children){
  int overPopSize = popSize + parentNum;

  // dump old pop and children into single array
  indv **overPop = malloc(sizeof(indv*)*overPopSize);
  memcpy(overPop,pop,sizeof(indv*)*(popSize));
  memcpy(overPop+(popSize),children,sizeof(indv*)*parentNum);

  // sort by fitness
  qsort(overPop,overPopSize,sizeof(indv*),cmpIndv);

  // put all but the last popSize*10% back into the population
  memcpy(pop,overPop,sizeof(indv*)*(popSize));

  killStragglers(overPop,overPopSize);

}

// recombination and mutation
// parameters:
//  p1 - parent 1
//  p2 - parent 2
//  child - child
void recombutate(int *p1, int *p2, int *child,int crossPoint){
  int i;

  for(i = 0; i <= crossPoint; i++){
    child[i] = p1[i];
  }
  for(i = 0; i < N; i++){
    if( !contains(child,p2[i],crossPoint+1) )
      child[++crossPoint] = p2[i];
  }

  if( getRand(100) < MUTATION){
    int x,y;
    x = getRand(N);
    do{
      y = getRand(N);
    } while (x == y);
    swap(child,x,y);
  }
}


// get the array of new children
// parameters:
// parents - array of parents
// returns: array of children
indv** breed(indv **parents){
  indv **children = malloc(sizeof(indv*)*parentNum);

  int i;
  for(i = 0; i < parentNum-1; i+=2){
    indv *c1 = malloc(sizeof(indv));
    indv *c2 = malloc(sizeof(indv));
    int *c1_Sol = malloc(sizeof(int)*N);
    int *c2_Sol = malloc(sizeof(int)*N);

    int crossPoint = getRand(N-1) + 1;

    recombutate(parents[i]->solution,parents[i+1]->solution,c1_Sol,crossPoint);
    recombutate(parents[i+1]->solution,parents[i]->solution,c2_Sol,crossPoint);

    c1->solution = c1_Sol;
    c2->solution = c2_Sol;
    c1->fitness = getFitValue(c1->solution);
    c2->fitness = getFitValue(c2->solution);
    children[i] = c1;
    children[i+1] = c2;
  }
  return children;
}

// determine the mating pool through tournament selection
// parameters:
//   pop - the array of individuals
// returns: array of parents that were selected
indv** dukeItOut(indv **pop){
  indv **parents = malloc(sizeof(indv*)*parentNum);
  int *usedArr = calloc(sizeof(int),(3*parentNum));
  int usedCount = 0;
//   printSol("u:",usedArr);

  int curr = 0;
  while(curr < parentNum){

    // pick 3 randomly (without replacement)
    int x,y,z;
    do{
      x = getRand(popSize);
    } while( contains(usedArr,x,usedCount) );
    usedArr[usedCount++] = x;
    do{
      y = getRand(popSize);
    } while( contains(usedArr,y,usedCount) );
    usedArr[usedCount++] = y;
    do{
      z = getRand(popSize);
    } while( contains(usedArr,z,usedCount) );
    usedArr[usedCount++] = z;

//     printf("\n%d\n%d\n%d\n",x,y,z);

    // select best based on fitness
    parents[curr] = pop[x];
    (parents[curr]->fitness < pop[y]->fitness) && (parents[curr] = pop[y]);
    (parents[curr]->fitness < pop[z]->fitness) && (parents[curr] = pop[z]);

    curr++;
  }
  free(usedArr);
  return parents;
}

// evaluate the fitness of each candidate
// parameters:
//  pop - array of candidates
void evalFitness(indv **pop){
  int i;
  for(i = 0; i < popSize; i++)
    pop[i]->fitness = getFitValue(pop[i]->solution);

}

// calculate the number of attacking Queens and invert
// parameters:
//  indv - individual to check
// returns: float fitness value
float getFitValue(int *soln){
  int i, j;
  float slope;
  int conflicts = 0;

  // compare positions pairwise, calculating slope between pairs
  // if slope is 1 or -1 there is a conflict
  for(i = 0; i < N - 1; i++){
    for(j = i + 1; j < N; j++){
      slope = ((soln[j]+1.0) - (soln[i]+1.0)) / ((j+1.0) - (i+1.0));
      if(slope == 1.0 || slope == -1.0){ conflicts++; }
    }
  }

  float fitness = (1 / (conflicts + 0.0001));
  if(fitness > 1){ solnFound = 1; }
  return fitness;
}

// initialize population array with random values
// returns: A double pointer to an array of candidate solutions
indv** initializePopulation(){
  int i, j, pos;
  int *soln;
  indv *ind;
  indv **pop = malloc(sizeof(indv*)*(popSize));


  for(i = 0; i < popSize; i++){
    ind = malloc(sizeof(indv));
    soln = malloc(sizeof(int)*N);

    // fill array 0-(N-1)
    for(j = 0; j < N; j++)
      soln[j] = j;

    shuffle(soln);
    ind->solution = soln;

    pop[i] = ind;
  }
  return pop;
}

// kill of the losers and the old pop
// parameters:
//  oldPop - old population pointer
//  overPop - array to free losers from
void killStragglers(indv **overPop,int size){
  int i;
  for(i = popSize; i < size; i++)
    free(overPop[i]);

  free(overPop);
}

// check if an array contains the given value
// parameters:
//  arr - array to check
//  val - value to check for
//  size - number of elements to check through
// returns: 1 if value found otherwise 0
int contains(int *arr, int val, int size){
  int i;
  for(i = 0; i < size; i++)
    if(arr[i] == val) return 1;
  return 0;
}

// shuffle an array randomly-ish
// parameters:
//  arr - int array to shuffle
void shuffle(int *arr){
  int i, j;
  for(i = 0; i < N - 1; i++){
    j = i + (rand() / (RAND_MAX / (N - i) + 1));
    swap(arr,i,j);
  }
}

// swap the values in the array at the positions given
// parameters:
//   indv - array to swap values in
//   x - position 1
//   y - position 2
void swap(int *soln, int x, int y){
  int temp;
  temp = soln[x];
  soln[x] = soln[y];
  soln[y] = temp;
}

// destroy array of pointers
// parameters:
//  arr - the pointer array
void destroyPop(indv **arr, int size){
  int i;
  for(i = 0; i < size; i++){
    free(arr[i]->solution);
    free(arr[i]);
  }
  free(arr);
}

// define cmp function for qsort
// parameters:
//  a - first thing to compare
//  b - second thing to compare
// returns: 1 if a > b
//          0 if a = b
//          1 if a < b
int cmpIndv(const void * a, const void * b){
  indv *a1 = *(indv* const *)a;
  indv *b1 = *(indv* const *)b;
  if(a1->fitness < b1->fitness)
    return 1;
  else if(a1->fitness > b1->fitness)
    return -1;
  else
    return 0;
}

// get a random number in a range
// parameters:
//  max - top bound on the number
// returns: number in bounds [0,max)
int getRand(int max){
  unsigned int x = (RAND_MAX + 1u) / max;
  unsigned int y = x * max;
  unsigned r;
  do {
    r = rand();
  } while( r >= y);
  return r / x;
}

// print the solutions
// parameters:
//  prompt - message before elements
//  arr - array to print
void printSol(char *prompt,int *arr){
  int i;

  printf("%s",prompt);
  for(i = 0; i < N; i++)
    printf(" %d",arr[i]);
  printf("\n");

}


// print the array
// parameters:
//  arr - array to print
void printPools(indv **arr, int size){
  int i;
  printf("\n");
  for(i = 0; i < size; i++){
    printf("Indv %d:",i+1);
    printSol("",arr[i]->solution);
  }
}

// print the fitness values
// parameters:
//  arr - array of individuals
void printFit(indv **arr, int size){
  int i,j;
  printf("\n");
  for(i = 0; i < size; i++){
    printf("Fit %d: %f\n",i+1,arr[i]->fitness);
  }
}
