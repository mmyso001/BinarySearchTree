/*
*Meekaeel Mysorewala
* Description: This program reads user input or a file input and outputs it to the screen or to an outputfile in ASCII order. 
* If -c is used the string is ordered case sensitive, if -o is used the output is sent to a file the user names on the command prompt. 
* In order to parse from the command line, getopt is used.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

// Node structure of BST
typedef struct TreeNode
{
    int count;
    char *string;
    struct TreeNode *leftChild;
    struct TreeNode *rightChild;
}TreeNode;


void Add(char *key, struct TreeNode **node, bool caseSensitiveFlag);
void CheckForWhiteSpaces(char *input);
void InOrder(struct TreeNode *, FILE *);
void DeleteBST(struct TreeNode **root);
void ProcessUserInput(struct TreeNode **root, bool caseSensitiveFlag);
bool CheckEmptyString(const char *input);
int StringComparisons(const char string1[], const char string2[], bool caseSensitiveFlag);

int main(int argc, char **argv)
{
    extern char *optarg;       
    extern int optind;
    extern int errno;          
    FILE *inputFile        = stdin; 
    FILE *outputFile       = stdout; 
    int input              = 0;
    int outputFlag         = 0; 
    int inputFlag          = 0;
    bool caseSensitiveFlag = false; 
    char inputFileName[128]; 
    char outputFileName[128];
    char bufferReader[400];   
    TreeNode *root = NULL; // root node holder

    static char usage[] = "Usage: %s bstsort [-c] [-o output_file_name] [input_file_name]\n";

    
    while ((input = getopt(argc,argv, "co:")) != -1)
    {
        
        switch (input)
        {
            case 'c':
                caseSensitiveFlag = true; // if -c activates case sensitivity
                break;
            case 'o':
                strcpy(outputFileName, optarg); // if -o reads to output file 
                outputFlag = 1;
                break;
            case '?':
                // Error
                fprintf(stderr,usage,argv[0]);
                exit(0);
        }
    }

    
    if (optind != argc)
    {
        strcpy(inputFileName, argv[optind]); // takes input file name 
        inputFlag = 1; 
    }

    // if input and output is entered 
    if(outputFlag && inputFlag)
    {
        
        if (inputFileName != NULL)
        {
            errno = 0; 

            inputFile = fopen(inputFileName, "r"); 

            if(!inputFile) // if the file input results in an error
            {
                fprintf(stderr, "Error: Couldn't open the file: %s\n", strerror(errno));
                exit(errno); //  closes the program
            }

            // Reads the file
            while(fgets(bufferReader, 400, inputFile) != NULL)
            {
                CheckForWhiteSpaces(bufferReader); 

                if (!CheckEmptyString(bufferReader)) 
                {
                    bufferReader[strcspn(bufferReader, "\n")] = '\0'; 
                    Add(bufferReader,  &root, caseSensitiveFlag); 
                }
            }
            fclose(inputFile); // close the file
        }

        // Writing File
        errno = 0;

        FILE *outputFile = fopen(outputFileName, "w"); // Writes output file

        if(!outputFile) // if it fails
        {
            fprintf(stderr, "Error: Unable to write the file: %s\n", strerror(errno));
            exit(errno); // close the file
        }

        InOrder(root, outputFile); 

        fclose(outputFile); // Close the file
    }
    else if (outputFlag == 1) // if only outputfile was given
    {
        ProcessUserInput(&root, caseSensitiveFlag); 

        errno = 0;

        FILE *outputFile = fopen(outputFileName, "w"); 

        if(!outputFile) 
        {
            fprintf(stderr, "Error: Couldn't open the file: %s\n", strerror(errno));
            exit(errno); 
        }

        InOrder(root, outputFile); 

        fclose(outputFile); // Close the file
    }
    else if (inputFlag == 1)  // if only input file was given
    {
        
        if (inputFileName != NULL)
        {
            errno = 0;

            inputFile = fopen(inputFileName, "r"); 

            if(!inputFile) 
            {
                fprintf(stderr, "Error: Unable to open the file: %s\n", strerror(errno));
                exit(errno); 
            }

            
            while(fgets(bufferReader, 400, inputFile) != NULL)
            {
                bufferReader[strcspn(bufferReader, "\n")] = '\0'; 
                Add(bufferReader,  &root, caseSensitiveFlag); 
            }
            fclose(inputFile); 
        }

        printf("\n"); 

        InOrder(root, outputFile); 
    }
    else 
    {
        ProcessUserInput(&root, caseSensitiveFlag); 
        printf("\n"); 
        InOrder(root, outputFile); 
    }

    DeleteBST(&root); // deletes the tree

    return 0;
}

// Compares the strings for both case sensitive and insensitive
int StringComparisons(const char *string1, const char *string2, bool caseSensitiveFlag)
{
    int length1 = strlen(string1) + 1; 
    int length2 = strlen(string2) + 1;
    int i;

    char string1Copy[length1]; 
	char string2Copy[length2];

	strcpy(string1Copy,string1); 
	strcpy(string2Copy,string2); 

    //compares lower case strings
    if (!caseSensitiveFlag)
    {
        for(i = 0; i < length1; i++)
        {
            string1Copy[i] = tolower(string1[i]); 
        }

        for(i = 0; i < length2; i++)
        {
            string2Copy[i] = tolower(string2[i]); 
        }
    }

    i = 0; 

    while(string1Copy[i] != '\0' && (string1Copy[i] == string2Copy[i]))
    {
        i++; 
    }

    if (string1Copy[i] == '\0') // strings are the same
    {
	    return 0;
    }
    else if (string1Copy[i] < string2Copy[i]) // 2 > 1
    {        
	return -1;
    }
    else
    { // 1 > 2
        return 1;
    }
}

//  traverses the tree in-order, and then outputs
void InOrder(struct TreeNode *root, FILE *stream)
{
    if (root != NULL) 
    {
        InOrder(root->leftChild, stream); // print left child first

        int i;

        for (i = 0; i < root->count; i++) 
        {
            fprintf(stream, "%s\n", root->string); 
        }

        InOrder(root->rightChild, stream); // print right child
    }
}

// takes user input and reads it to tree
void ProcessUserInput(struct TreeNode **root, bool isCaseSensitive)
{
  char input[129]; 

  printf("\nEnter a sentence, if you wish to finish or cancel press enter with no input. : ");

  
  while ((fgets(input, sizeof(input), stdin) != NULL) && (input[0] != '\n'))
  {
      CheckForWhiteSpaces(input); 

      if (!CheckEmptyString(input)) 
      {
        input[strcspn(input, "\n")] = '\0'; 
        Add(input,  root, isCaseSensitive); 
        printf("\nEnter another sentence or Press enter to finish: "); 
      }
      else
      {
        printf("\nYou entered an empty line. Try again or Press enter to finish: "); 
      }
  }
}

// traverses the tree in post order, then removes it
void DeleteBST(struct TreeNode **root)
{
    if(*root != NULL) 
    {
        DeleteBST(&(*root)->leftChild);
        DeleteBST(&(*root)->rightChild);
        free((*root)->string);
        free((*root)); 
    }

}

// adds a node to the bst
void Add(char *key, struct TreeNode **node, bool isCaseSensitive)
{
    if(*node == NULL) 
    {
        *node = malloc(sizeof(struct TreeNode)); 
        (*node)->string = malloc(sizeof(char) * strlen(key) + 1); 
        memcpy((*node)->string, key, strlen(key) + 1); 
        (*node)->count = 1; 
        (*node)->leftChild = NULL; 
        (*node)->rightChild = NULL;
    }
    else
    {
        int result;

        result = StringComparisons(key, (*node)->string, isCaseSensitive); 

        if(result < 0)
	{
            Add(key, &(*node)->leftChild, isCaseSensitive);
	}
        else if (result > 0) 
	{
            Add(key, &(*node)->rightChild, isCaseSensitive);
	}
        else
	{
            (*node)->count += 1; 
	}
    }
}

// Trims all the whitespace 
void CheckForWhiteSpaces(char *input)
{
    int i;
    int startingPoint = 0;
    int endingPoint = strlen(input) - 1;

    while(isspace((unsigned char)input[startingPoint])) 
        startingPoint++; 

    
    while((endingPoint >= startingPoint) && isspace((unsigned char) input[endingPoint]))
        endingPoint--;

    for(i = startingPoint; i <= endingPoint; i++)
        input[i - startingPoint] = input[i]; 

    input[i - startingPoint] = '\0';
}

// checks for empty input lines
bool CheckEmptyString(const char *input)
{
    while(*input != '\0') 
    {
        if(!isspace((unsigned char) *input)) 
            return false; 

        input++; 
    }
    return true;
}
