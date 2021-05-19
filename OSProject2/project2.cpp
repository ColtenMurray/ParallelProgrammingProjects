#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

static int seed;
static bool isGameOver;
static int DiceSum_A;
static int DiceSum_B;
static int DiceSum_C;
static int DiceSum_D;



pthread_mutex_t winning_mutex;
pthread_mutex_t A_mutex;
pthread_mutex_t B_mutex;
pthread_mutex_t C_mutex;
pthread_mutex_t D_mutex;

pthread_cond_t game_won;
pthread_cond_t A_turn;
pthread_cond_t B_turn;
pthread_cond_t C_turn;
pthread_cond_t D_turn;

using namespace std;

static void* rollDice(void* arg)
{
    const long my_rank = (long)arg;
    printf("thread %d running rollDice\n", (int)my_rank);

    int DiceRoll_1;
    int DiceRoll_2;
    int DiceSum;
    ofstream output;

    while(!isGameOver)
    {
        switch(my_rank)
        {
        case 0:
            pthread_mutex_lock(&A_mutex);
            pthread_cond_wait(&A_turn,&A_mutex);


            if(!isGameOver)
            {

                DiceRoll_1 = rand()%6 + 1;
                DiceRoll_2 = rand()%6 + 1;
                DiceSum = DiceRoll_1 + DiceRoll_2;
                DiceSum_A = DiceSum;

                output.open("output.txt", ios::out | ios::app);
                output << "PLAYER A: gets " << DiceRoll_1 << " and " << DiceRoll_2 << " with a sum " << DiceSum << "\n";
                output.close();
                printf("PLAYER A: %d %d \n",DiceRoll_1,DiceRoll_2);

                if(DiceSum_A == DiceSum_C)
                {
                    isGameOver = true;
                    pthread_mutex_lock(&winning_mutex);
                    pthread_cond_signal(&game_won);
                    pthread_mutex_unlock(&winning_mutex);
                }
            }

            pthread_mutex_lock(&B_mutex);
            pthread_cond_signal(&B_turn);
            pthread_mutex_unlock(&B_mutex);

            pthread_mutex_unlock(&A_mutex);

            break;

        case 1:
            pthread_mutex_lock(&B_mutex);
            pthread_cond_wait(&B_turn,&B_mutex);

            if(!isGameOver)
            {
                DiceRoll_1 = rand()%6 + 1;
                DiceRoll_2 = rand()%6 + 1;
                DiceSum = DiceRoll_1 + DiceRoll_2;
                DiceSum_B = DiceSum;

                output.open("output.txt", ios::out | ios::app);
                output << "PLAYER B: gets " << DiceRoll_1 << " and " << DiceRoll_2 << " with a sum " << DiceSum << "\n";
                output.close();
                printf("PLAYER B: %d %d \n",DiceRoll_1,DiceRoll_2);

                if(DiceSum_B == DiceSum_D)
                {
                    isGameOver = true;
                    pthread_mutex_lock(&winning_mutex);
                    pthread_cond_signal(&game_won);
                    pthread_mutex_unlock(&winning_mutex);
                }
            }

            pthread_mutex_lock(&C_mutex);
            pthread_cond_signal(&C_turn);
            pthread_mutex_unlock(&C_mutex);

            pthread_mutex_unlock(&B_mutex);
            break;
        case 2:
            pthread_mutex_lock(&C_mutex);
            pthread_cond_wait(&C_turn,&C_mutex);

            if(!isGameOver)
            {

                DiceRoll_1 = rand()%6 + 1;
                DiceRoll_2 = rand()%6 + 1;
                DiceSum = DiceRoll_1 + DiceRoll_2;
                DiceSum_C = DiceSum;

                output.open("output.txt", ios::out | ios::app);
                output << "PLAYER C: gets " << DiceRoll_1 << " and " << DiceRoll_2 << " with a sum " << DiceSum << "\n";
                output.close();
                printf("PLAYER C: %d %d\n",DiceRoll_1,DiceRoll_2);

                if(DiceSum_A == DiceSum_C)
                {
                    isGameOver = true;
                    pthread_mutex_lock(&winning_mutex);
                    pthread_cond_signal(&game_won);
                    pthread_mutex_unlock(&winning_mutex);
                }
            }

            pthread_mutex_lock(&D_mutex);
            pthread_cond_signal(&D_turn);
            pthread_mutex_unlock(&D_mutex);

            pthread_mutex_unlock(&C_mutex);
            break;
        case 3:
            pthread_mutex_lock(&D_mutex);
            pthread_cond_wait(&D_turn,&D_mutex);

            if(!isGameOver)
            {

                DiceRoll_1 = rand()%6 + 1;
                DiceRoll_2 = rand()%6 + 1;
                DiceSum = DiceRoll_1 + DiceRoll_2;
                DiceSum_D = DiceSum;

                output.open("output.txt", ios::out | ios::app);
                output << "PLAYER D: gets " << DiceRoll_1 << " and " << DiceRoll_2 << " with a sum " << DiceSum << "\n";
                output.close();
                printf("PLAYER D: %d %d\n",DiceRoll_1,DiceRoll_2);

                if(DiceSum_B == DiceSum_D)
                {
                    isGameOver = true;
                    pthread_mutex_lock(&winning_mutex);
                    pthread_cond_signal(&game_won);
                    pthread_mutex_unlock(&winning_mutex);
                }
            }

            pthread_mutex_lock(&A_mutex);
            pthread_cond_signal(&A_turn);
            pthread_mutex_unlock(&A_mutex);

            pthread_mutex_unlock(&D_mutex);
            break;
        }
    }
    pthread_exit(NULL);
}

static void* deal(void* arg)
{
    ofstream output;
    const long my_rank = (long)arg;
    printf("thread %d running deal\n", (int)my_rank);

    pthread_mutex_lock(&winning_mutex);

    pthread_mutex_lock(&A_mutex);
    pthread_cond_signal(&A_turn);
    pthread_mutex_unlock(&A_mutex);

    pthread_cond_wait(&game_won,&winning_mutex);
    if(DiceSum_A == DiceSum_C)
    {
        output.open("output.txt", ios::out | ios::app);
        output << "DEALER: The winning team is A and C\n";
        output.close();
        printf("DEALER: The winning team is A and C\n");
    }
    else
    {
        output.open("output.txt", ios::out | ios::app);
        output << "DEALER: The winning team is B and D\n";
        output.close();

        printf("DEALER: The winning team is B and D\n");
    }
    pthread_mutex_unlock(&winning_mutex);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{

  // check command line
  if (argc != 2) {fprintf(stderr, "USAGE: %s seed\n", argv[0]); exit(-1);}
  seed = atoi(argv[1]);
  srand(seed);

  ofstream output;
  output.open("output.txt",ios::out | ios::trunc);
  output << "Matching Dice!\n";
  output.close();
  printf("Matching Dice!\n");

  long threads = 5;
  isGameOver = false;



  // initialize pthread variables
  pthread_t* const handle = new pthread_t [threads];

  // launch threads
  for(int thread = 0; thread < threads -1; thread++)
  {
      if( pthread_create(&handle[thread], NULL, rollDice, (void *)thread) != 0)
      {
          printf("Error creating Player thread %d...\n", thread);
      }
  }
  sleep(1);
  if(pthread_create(&handle[threads-1], NULL, deal, (void *)threads-1) != 0)
  {
      printf("Error creating Dealer thread...\n");
  }


  // join threads
  for(int thread = 0; thread < threads; thread++)
  {
      pthread_join(handle[thread], NULL);
  }

  output.close();
  // cleanup
  pthread_mutex_destroy(&winning_mutex);
  pthread_mutex_destroy(&A_mutex);
  pthread_mutex_destroy(&B_mutex);
  pthread_mutex_destroy(&C_mutex);
  pthread_mutex_destroy(&D_mutex);
  pthread_cond_destroy(&game_won);
  pthread_cond_destroy(&A_turn);
  pthread_cond_destroy(&B_turn);
  pthread_cond_destroy(&C_turn);
  pthread_cond_destroy(&D_turn);
  pthread_exit(NULL);

}
