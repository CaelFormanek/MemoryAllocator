#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* function declarations */
void runProgram(char** memory);
int handleRQ(char* split_args[4], char** memory);
int handleRL(char* split_args[4], char** memory);
int handleC(char* split_args[4], char** memory);
int handleSTAT(char* split_args[4], char** memory);
int handleX(char* split_args[4], char** memory);
int delegateHandler(char* split_args[4], char** memory);

/* global variables */
int num_bytes_memory = 0;

int main(int argc, char** argv)
{
  /* get command line arguments */
  if (argc != 2)
  {
    printf("You must provide the name of the program and initial amount of memory at startup, nothing more or less.\n");
    return -1;
  }

  int a = 0;
  for (a = 0; a < strlen(argv[1]); a++)
  {
    if (!isdigit(argv[1][a]))
    {
      printf("You entered %s, but you must enter an integer number (no decimals or extra characters).\n", argv[1]);
      return -1;
    }
  }

  /* allocate the memory */
  int b = 0;
  num_bytes_memory = atoi(argv[1]);
  char** memory = (char**)malloc((sizeof(char*))* num_bytes_memory);
  for (b = 0; b < num_bytes_memory; b++)
  {
    memory[b] = NULL;
  }

  /* run the program */
  runProgram(memory);

  /* memory cleanup */
  int c = 0;
  for (c = 0; c < num_bytes_memory; c++)
  {
    if (memory[c] != NULL)
    {
      free(memory[c]);
    }
  }
  
  return 0;
}

void runProgram(char** memory)
{
  /* possible commands and strategies */
  char* possible_commands[5] = {"RQ", "RL", "C", "STAT", "X"};

  /* variables user_input and split_argument_arr */
  char* user_input = (char*)malloc(sizeof(char) * 50);
  char* split_argument_arr[4] = {NULL};
  
  while (1)
  {
    printf("allocator>");
    fgets(user_input, 50, stdin);
    
    /* split up string into individual parts */
    int index_counter = 0;
    char* temp_str = strtok(user_input, " ");
    do 
    {
      if (temp_str[strlen(temp_str)-1] == '\n')
      {
        temp_str[strlen(temp_str)-1] = '\0';
      }
      split_argument_arr[index_counter] = (char*)(malloc(strlen(temp_str)));
      strcpy(split_argument_arr[index_counter], temp_str);
      temp_str = strtok(NULL, " ");
      ++index_counter;
    } while(temp_str != NULL );

    /* make sure user entered value command */
    int i, non_matches = 0;
    for (i = 0; i < 5; i++)
    {
      if (strcmp(split_argument_arr[0], possible_commands[i]) != 0)
      {
        ++non_matches;
      }
    }
    if (non_matches == 5)
    {
      printf("Command %s is an invalid first command. If you entered a command in lowercase, check case sensitivity\n", split_argument_arr[0]);
      int clear = 0;
      for (clear = 0; clear < 4; clear ++)
      {
        split_argument_arr[clear] = NULL;
      }
      continue;
    }

    if (delegateHandler(split_argument_arr, memory) == -1)
    {
      int i = 0;
      for (i = 0; i < 4; i++)
      {
        if (split_argument_arr[i] != NULL)
        {
          free(split_argument_arr[i]);
        }
      }
      
      break;
    }
  }
  if (user_input != NULL)
  {
    free(user_input);
  }
}

int handleRQ(char* split_args[4], char** memory)
{
  int clear = 0;

  /* four arguments */
  int a = 0;
  for (a = 0; a < 4; a++)
  {
    if (split_args[a] == NULL)
    {
      printf("To use RQ, you must use the following format:\n");
      printf("RQ <process> <number of bytes requested> <memory allocation strategy>\n");
      for (clear = 0; clear < 4; clear ++)
      {
        split_args[clear] = NULL;
      }
      return 0;
    }
  }

  /* processes in memory */
  int b = 0;
  for (b = 0; b < num_bytes_memory; b++)
  {
    if (memory[b] != NULL)
    {
      if (strcmp(memory[b], split_args[1]) == 0)
      {
        printf("Process %s already exists in memory\n", split_args[1]);
        for (clear = 0; clear < 4; clear ++)
        {
          split_args[clear] = NULL;
        }
        return 0;
      }
    }
  }

  /* third argument should be a number */
  int c = 0;
  for (c = 0; c < strlen(split_args[2]); c++)
  {
    if (!isdigit(split_args[2][c]))
    {
      printf("You entered %s, but you must enter an integer number (no decimals or extra characters).\n", split_args[2]);
      for (clear = 0; clear < 4; clear ++)
      {
        split_args[clear] = NULL;
      }
      return 0;
    }
  }

  /* implement request */
  if (strcmp(split_args[3], "F") == 0)
  {
    /* implement F logic*/
    int num_consecutive_null_spaces = 0;
    int new_starting_index = 0;

    int i = 0;
    int j = 0;
    for (i = 0; i < num_bytes_memory; i++)
    {
      /* if memory is null, it is a free block */
      if (memory[i] == NULL)
      {
        ++num_consecutive_null_spaces;
        if (num_consecutive_null_spaces == atoi(split_args[2]) )
        {
          for (j = new_starting_index; j < (new_starting_index + num_consecutive_null_spaces); j++)
          {
            memory[j] = strdup(split_args[1]);
          }
          printf("Allocated first open %d bytes to %s, starting at index %d ending at index %d\n", num_consecutive_null_spaces, split_args[1], new_starting_index, (new_starting_index + num_consecutive_null_spaces - 1));
          for (clear = 0; clear < 4; clear ++)
          {
            split_args[clear] = NULL;
          }
          return 0;
        }
      }
      else
      {
        /* start the counting over*/
        if ((i+1) != num_bytes_memory)
        {
          new_starting_index = i+1;
        }
        num_consecutive_null_spaces = 0;
      }
    }
    printf("There is not enough contigous memory to allocate %d bytes to process %s\n", atoi(split_args[2]), split_args[1]);
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }
  else if (strcmp(split_args[3], "B") == 0)
  {
    /* implement B logic */
    int num_consecutive_null_spaces = 0;
    int new_starting_index = 0;
    int final_starting_index = 0;
    int final_num_consecutive_null_spaces = 0;
    int current_smallest_diff = num_bytes_memory;

    int i = 0;
    int j = 0;
    for (i = 0; i < num_bytes_memory; i++)
    {
      /* a NULL space is not full */
      if (memory[i] == NULL)
      {
        ++num_consecutive_null_spaces;
        int diff = atoi(split_args[2]) - num_consecutive_null_spaces;
        if (diff < 0)
        {
          diff *= -1;
        }
        /* calculate starting position of smallest hole */
        if ((new_starting_index == final_starting_index) && (num_consecutive_null_spaces > final_num_consecutive_null_spaces))
        {
          current_smallest_diff = diff;
        }
        if ((num_consecutive_null_spaces >= atoi(split_args[2])) && (diff < current_smallest_diff))
        {
          final_num_consecutive_null_spaces = num_consecutive_null_spaces;
          final_starting_index = new_starting_index;
          current_smallest_diff = diff;
        }
      }
      else
      {
        /* start over if space is full */
        if ((i+1) != num_bytes_memory)
        {
          new_starting_index = i+1;
        }
        num_consecutive_null_spaces = 0;
      }
    }

    /* allocate bytes based on calculated index */
    if (final_num_consecutive_null_spaces >= atoi(split_args[2]))
    {
      for (j = final_starting_index; j < (final_starting_index + final_num_consecutive_null_spaces); j++)
      {
        memory[j] = strdup(split_args[1]);
      }
      printf("Allocated %d bytes to %s, starting at index %d ending at index %d\n", final_num_consecutive_null_spaces, split_args[1], final_starting_index, (final_starting_index + final_num_consecutive_null_spaces - 1));
      for (clear = 0; clear < 4; clear ++)
      {
        split_args[clear] = NULL;
      }
      return 0;
    }
    else
    {
      printf("There is not enough contigous memory to allocate %d bytes to process %s\n", atoi(split_args[2]), split_args[1]);
      for (clear = 0; clear < 4; clear ++)
      {
        split_args[clear] = NULL;
      }
      return 0;
    }
  }
  else if (strcmp(split_args[3], "W") == 0)
  {
    /* implement W logic */
    int num_consecutive_null_spaces = 0;
    int new_starting_index = 0;
    int final_starting_index = 0;
    int final_num_consecutive_null_spaces = 0;
    int current_largest_diff = 0;

    int i = 0;
    int j = 0;
    for (i = 0; i < num_bytes_memory; i++)
    {
      /* NULL space is not full */
      if (memory[i] == NULL)
      {
        ++num_consecutive_null_spaces;
        int diff = atoi(split_args[2]) - num_consecutive_null_spaces;
        if (diff < 0)
        {
          diff *= -1;
        }
        if ((num_consecutive_null_spaces >= atoi(split_args[2])) && (diff >= current_largest_diff))
        {
          final_num_consecutive_null_spaces = num_consecutive_null_spaces;
          final_starting_index = new_starting_index;
          current_largest_diff = diff;
        }
      }
      else
      {
        if ((i+1) != num_bytes_memory)
        {
          new_starting_index = i+1;
        }
        num_consecutive_null_spaces = 0;
      }
    }

    /* allocate bytes */
    if (final_num_consecutive_null_spaces >= atoi(split_args[2]))
    {
      for (j = final_starting_index; j < (final_starting_index + atoi(split_args[2])); j++)
      {
        memory[j] = strdup(split_args[1]);
      }
      printf("Allocated %d bytes to %s, starting at index %d ending at index %d within largest hole [%d:%d]\n", atoi(split_args[2]), split_args[1], final_starting_index, (final_starting_index + atoi(split_args[2]) - 1), final_starting_index, final_starting_index+final_num_consecutive_null_spaces-1);
      for (clear = 0; clear < 4; clear ++)
      {
        split_args[clear] = NULL;
      }
      return 0;
    }
    else
    {
      printf("There is not enough contigous memory to allocate %d bytes to process %s\n", atoi(split_args[2]), split_args[1]);
      for (clear = 0; clear < 4; clear ++)
      {
        split_args[clear] = NULL;
      }
      return 0;
    }
  }
  else
  {
     printf("%s is an invalid strategy. Enter F, B, or W. If you entered a strategy in lowercase, check case sensitivity\n", split_args[3]);
     for (clear = 0; clear < 4; clear ++)
     {
      split_args[clear] = NULL;
     }
     return 0;
  }
}

int handleRL(char* split_args[4], char** memory)
{
  int clear = 0;
  if (split_args[1] == NULL)
  {
    printf("To use RL, you must use the following format:\n");
    printf("RL <process to be released>\n");
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }

  int i = 0;
  int num_space_currently_taken_by_process = 0;
  int start_index = num_bytes_memory;
  int end_index = 0;
  
  /* search through entire memory for process */
  for (i = 0; i < num_bytes_memory; i++)
  {
    /* if process is found, make it NULL*/
    if (memory[i] != NULL)
    {
      if (strcmp(memory[i], split_args[1]) == 0)
      {
        memory[i] = NULL;
        if (i < start_index)
        {
          start_index = i;
        }
        ++num_space_currently_taken_by_process;
        end_index = start_index + num_space_currently_taken_by_process -1;
      }
    }
  }
  /* no such process is in memory */
  if (num_space_currently_taken_by_process == 0)
  {
    printf("The process %s does not exist in memory\n", split_args[1]);
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }
  printf("Released %s from index %d to index %d\n", split_args[1], start_index, end_index);
  for (clear = 0; clear < 4; clear ++)
  {
    split_args[clear] = NULL;
  }
  return 0;
}

int handleC(char* split_args[4], char** memory)
{
  int clear = 0;
  if (split_args[1] != NULL)
  {
    printf("To use C, C should be the only argument\n");
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }

  int new_index = 0;
  int i = 0;

  /* compaction may have already been achieved */
  int num_fragments = 0;
  char* marker_str = "placeholder";
  for (i = 0; i < num_bytes_memory; i++)
  {
    if ((memory[i] == NULL) && (marker_str != NULL))
    {
      ++num_fragments;
      marker_str = NULL;
    }
    if (memory[i] != NULL)
    {
      marker_str = strdup(memory[i]);
    }
  }
  if (num_fragments < 2)
  {
    printf("Compaction has already been achieved, there are 1 or less contiguous memory holes. Enter STAT to see\n");
    return 0;
  }

  /* put process in previous memory */
  for (i = 0; i < num_bytes_memory; i++)
  {
    if (memory[i] != NULL)
    {
      memory[new_index] = strdup(memory[i]);
      if (new_index != i )
      {
        memory[i] = NULL;
      }
      new_index++;
    }
  }

  for (clear = 0; clear < 4; clear ++)
  {
    split_args[clear] = NULL;
  }
  return 0;
}

int handleSTAT(char* split_args[4], char** memory)
{
  int clear = 0;
  if (split_args[1] != NULL)
  {
    printf("To use STAT, STAT should be the only argument\n");
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }

  /* only 1 byte of memory */
  if ((num_bytes_memory == 1) && (memory[0] != NULL))
  {
    printf("Addresses [0:0] Process %s\n", memory[0]);
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }

  int i = 0;
  int start_index = 0;
  int end_index = 0;
  char* marker_str = memory[0];

  for (i = 0; i < num_bytes_memory; i++)
  {
    /* memory is NULL and previous space was NULL */
   if ((memory[i] == NULL) && (marker_str == NULL))
   {
    end_index = i;
    if (end_index == num_bytes_memory-1)
    {
      printf("Addresses [%d:%d] Unused\n", start_index, end_index);
    }
   }
   /* memory is filled and previous space was NULL */
   else if ((memory[i] != NULL) && (marker_str == NULL))
   {
    printf("Addresses [%d:%d] Unused\n", start_index, end_index);
    marker_str = strdup(memory[i]);
    start_index = i;
    end_index = i;
   }
   /* memory is NULL and previous space was filled */
   else if ((memory[i] == NULL) && (marker_str != NULL))
   {
    printf("Addresses [%d:%d] Process %s\n", start_index, end_index, marker_str);
    marker_str = NULL;
    start_index = i;
    end_index = i;
    if (i == num_bytes_memory-1)
    {
      printf("Addresses [%d:%d] Unused\n", start_index, end_index);
    }
   }
   /* memory is filled and previous space was filled */
   else if ((memory[i] != NULL) && (marker_str != NULL))
   {
    if (strcmp(memory[i], marker_str) == 0)
    {
      end_index = i;
    }
    else
    {
      printf("Addresses [%d:%d] Process %s\n", start_index, end_index, marker_str);
      marker_str = memory[i];
      start_index = i;
      end_index = i;
    }
    if (i == num_bytes_memory-1)
    {
      printf("Addresses [%d:%d] Process %s\n", start_index, end_index, marker_str);
    }
   }
  }
  for (clear = 0; clear < 4; clear ++)
  {
    split_args[clear] = NULL;
  }
  return 0;
}

int handleX(char* split_args[4], char** memory)
{
  int clear = 0;
  if (split_args[1] != NULL)
  {
    printf("To use X, X should be the only argument\n");
    for (clear = 0; clear < 4; clear ++)
    {
      split_args[clear] = NULL;
    }
    return 0;
  }
  for (clear = 0; clear < 4; clear ++)
  {
    split_args[clear] = NULL;
  }
  /* exit the program */
  return -1;
}

int delegateHandler(char* split_args[4], char** memory)
{
  if (strcmp(split_args[0], "RQ") == 0)
  {
    return handleRQ(split_args, memory);
  }
  else if (strcmp(split_args[0], "RL") == 0)
  {
    return handleRL(split_args, memory);
  }
  else if (strcmp(split_args[0], "C") == 0)
  {
    return handleC(split_args, memory);
  }
  else if (strcmp(split_args[0], "STAT") == 0)
  {
    return handleSTAT(split_args, memory);
  }
  else if (strcmp(split_args[0], "X") == 0)
  {
    return handleX(split_args, memory);
  }
  return 0;
}
