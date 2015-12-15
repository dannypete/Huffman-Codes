/*****************************************/
/* Daniel Peterson                       */
/* December 4, 2015                      */
/* CS-241 Section #3                     */
/*                                       ********************/
/* This program uses the Huffman coding technique in order  */
/* to compress files.                                       */
/* For more information of the Huffman technique, see:      */
/*     https://en.wikipedia.org/wiki/Huffman_coding         */
/*                                                          */
/* The program expects two command line arguments. The      */
/* first is the file to be encoded. The second is the file  */
/* to be created which will contain the encoded data        */
/* It returns errors for invalid argument number, problems  */
/* opening or closing files, etc.                           */
/************************************************************/

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

/* Max tree depth and therefore max code length */
#define maxHeight 127

/* Holds character frequencies of characters in the input stream */
int frequencyMap[256] = {0};

/* Structure to hold Huffman codes */
int huffmanValues[256][maxHeight];

/* Does-it-all Node */
struct QueueNode 
{
  /* The ASCII character*/
  int data;
  
  /* The character's frequency */
  int frequency;
  
  /* Left child of current node */
  struct QueueNode* left;
  
  /* Right child of current node */
  struct QueueNode* right;
  
  /* Next node in list */
  struct QueueNode* next;
};

struct bitNode
{
  /* The particular bit (1 or 0) */
  int bit;

  /* The next bit */
  struct bitNode* nextBit;
};

/* Scans the "file" (stdin), adds occurrances to frequencyMap */
void countFrequencies(FILE* in)
{
  /* char c */
  int c;

  /* While not EOF, get next character, add to frequencyMap */
  while((c = getc(in)) != EOF) 
  {
    if(c > 256 || c < 0) printf("What is this?? %d=%c\n", c, c);
    else frequencyMap[c]++;
  }
}

/************************************************************************************
 * struct QueueNode* insertSorted(struct QueueNode* head, struct QueueNode* newNode)
 *
 * The function takes a new node, newNode, and inserts it into the correct position
 * in the existing linked list, starting at head. Returns the (maybe new) head.
 */ 
struct QueueNode* insertSorted(struct QueueNode* head, struct QueueNode* newNode)
{
  struct QueueNode* current = head;
  
  /* If the list-tree is empty or the new node's frequency is less than 
     the smallest (head) node's frequency, add it there */
  if(head == NULL || newNode->frequency < head->frequency)
  {
    newNode->next = current;
    return newNode;
  }

  /* Keep checking that there is another node after current node and next node 
     freq is less than new node's freq, then put the new node at the current 
     position (where either of these are untrue: end of list or 
     new node < existing node) */

  else
  {
    /* Comparing unequal frequencies */
    while(current->next != NULL && current->next->frequency < newNode->frequency) 
      current = current->next;

    if(head->frequency == newNode->frequency && head->data > newNode->data)
    {
      newNode->next = current;
      return newNode;
    }

    /* Comparing unequal data (ASCII) values with equal frequency values */
    while(current->next != NULL && current->next->frequency == newNode->frequency && 
	  current->next->data < newNode->data) 
      current = current->next;
  }
  newNode->next = current->next;
  current->next = newNode;
  return head;
}
/**************************************************************************************
 * struct QueueNode* createNodeLinked(struct QueueNode* head, int data, int frequency)
 *
 * Creates a node with given data, data, and frequency, frequency, and inserts it 
 * in the correct location. Returns the head.
 */ 
struct QueueNode* createNodeLinked(struct QueueNode* head, int data, int frequency)
{
  /* Make space for new node to insert */
  struct QueueNode* newNode = malloc(sizeof(struct QueueNode));

  /* Initialize new node's values */
  newNode->data = data;
  newNode->frequency = frequency;
  newNode->left = NULL;
  newNode->right = NULL;
  newNode->next = NULL;

  return insertSorted(head, newNode);
}

/*********************************************************************************************
 * void printQueue(struct QueueNode* head)
 * 
 * Internal function used for debuggging.
 * Prints the linked list.
 */
void printQueue(struct QueueNode* head)
{
  /* Node for iterating */
  struct QueueNode* current = head;
  
  /* Iterating while the current node isn't null to print values for debugging */
  while(current != NULL)
  {
    if(current->data < 33 || current->data > 126) 
      printf("=%d occurred %d times\n", current->data, current->frequency);
    
    else printf("%c occurred %d times\n", current->data, current->frequency);
    current = current->next;
  }
  printf("\n");
}   

/*************************************************************
 * void freeQueue(struct QueueNode* head)
 *
 * Unused function. Frees a linkedlist at
 * head. 
 */
void freeQueue(struct QueueNode* head)
{
  struct QueueNode* current = head;
  if(current->next != NULL)
  {
    freeQueue(current->next);
    current->next = NULL;
    free(current);
  }
}


/*********************************************************************************
 * struct QueueNode* createNodeTree (struct QueueNode* head)
 *
 * Makes a node that is the root of two or more combined
 * nodes when building the Huffman tree. Takes the first 
 * nodes from a sorted list, starting at head. Returns the
 * new head.
 */
struct QueueNode* createNodeTree (struct QueueNode* head)
{
  /* Make space for new node to insert (root node of two combined nodes) */
  struct QueueNode* newNode = malloc(sizeof(struct QueueNode));

  /* Make the ndoe to be the new head */
  struct QueueNode* newHead;  
  
  /* Initialize values of the new node, change pointers of children */
  newNode->data = head->next->data;
  newNode->frequency = (head->frequency) + (head->next->frequency);
  newNode->left = head;
  newNode->right = head->next;
  newNode->next = NULL;

  newHead = head->next->next;
  head->next->next = NULL;
  head->next = NULL;
  
  /* Insert new root with children back into queue */
  return insertSorted(newHead, newNode);
	
}

/****************************************************************
 * struct QueueNode* buildTree(struct QueueNode* head)
 *
 * Calls createNodeTree on a queue at head, which combines two 
 * nodes until only one remains and a huffman tree is formed.
 * Returns the tree root.
 */
struct QueueNode* buildTree(struct QueueNode* head)
{ 
  while(head->next != NULL)
    head = createNodeTree(head);

  return head;
}

/***************************************************************************
 * void printTree(struct QueueNode* head)
 *
 * Internal function used for debugging. Prints the tree
 * which starts at head, in-order.
 */
void printTree(struct QueueNode* head)
{
  if(head != NULL)
  {
    printTree(head->left);

    /* If this isn't a leaf/child, incidate so */
    if(head->left != NULL && head->right != NULL) 
      printf("Parent of left:%d%c and right:%d%c; node:%d%c\n", 
	     head->left->frequency, head->left->data, 
	     head->right->frequency, head->right->data, 
	     head->frequency, head->data);

    /* Its a child/leaf, so indicate its values */
    else
    {
      if(head->data < 33 || head->data > 126) 
	printf("=%d occurred %d times\n", head->data, head->frequency);
      
      else printf("%c occurred %d times\n", head->data, head->frequency);
    }

    printTree(head->right);
  }
}

/*****************************************************************************
 * void freeTree(struct QueueNode* head)
 * 
 * Frees the tree starting at head from memory.
 */
void freeTree(struct QueueNode* head)
{
  if(head != NULL)
  {
    freeTree(head->left);
    freeTree(head->right);
    free(head);
  }
}

/********************************************************
 * void storeCodes(int arr[], int n, int data)
 *
 * Calculates the huffman codes and puts them in the
 * huffmanValues array. arr is one specific huffman
 * value, n is the length of the specific huffman code,
 * and data is the character the code represents.
 */
void storeCodes(int arr[], int n, int data)
{
  int i;
  
  /* First array value is how long the huffman code is
     for later book-keeping */
  huffmanValues[data][0] = n;

  /* The rest are the actual huffman code */
  for(i = 0; i < n; i++)
    huffmanValues[data][i + 1] = arr[i];
}

/*****************************************************************
 * void printDataValues()
 *
 * Prints out the character, frequency, and huffman code for
 * each character that shows up in the huffman tree. Puts out to
 * stdout.
 */
void printDataValues()
{
  int i, j;
  unsigned short count;

  printf("Symbol\tFreq\tCode\n");

  for(i = 0; i < 256; i++)
  {
    /* Only print the symbols that appear at least once */
    if(frequencyMap[i] > 0)
    {
      count++;

      /* It's non-printing ASCII */
      if(i < 33 || i > 126) printf("=%d\t%d\t", i, frequencyMap[i]);

      /* It's printing ASCII */
      else printf("%c\t%d\t", i, frequencyMap[i]);
      
      for(j = 1; j < huffmanValues[i][0] + 1; j++) 
	printf("%d", huffmanValues[i][j]);
      
      printf("\n");
    }
  }
}

/************************************************************************* 
 * void generateCodes(struct QueueNode* root, int arr[], int top)
 *
 * Generates the huffman codes with the given tree at root.
 * arr holds the current path that has been taken along the tree,
 * and top holds the current index where the next bit of the code will
 * go in arr.
 */
void generateCodes(struct QueueNode* root, int arr[], int top)
{
  /* Still traversing/recurring because it's not a leaf */
  if(root->left)
  {
    arr[top] = 0;
    generateCodes(root->left, arr, top + 1);
  }

  if(root->right)
  {
    arr[top] = 1;
    generateCodes(root->right, arr, top + 1);
  }

  /* It is a leaf, store is huffman value */
  if(root->left == NULL && root->right == NULL)
  {
    storeCodes(arr, top, root->data);
  }
}

/*********************************************************************
 * void generateCodesHelper(struct QueueNode* root)
 *
 * Helper function to generateCodes. Used to initialize arr[] and top.
 * root is the root of the huffman tree.
 */
void generateCodesHelper(struct QueueNode* root)
{
  int arr[maxHeight], top = 0;
  generateCodes(root, arr, top);
}

/**********************************************************************
* struct bitNode* createNodeInsert(struct bitNode* head, int data)
*
* Creates a bitNode and inserts it at the end of the currently 
* existing bitNodes. Data is the bit value of the node, and head is
* this head of the bitNode linked list.
*/
struct bitNode* createNodeInsert(struct bitNode* head, int data)
{
  /* Node for iterating */
  struct bitNode* current = head;

  /* New node to be added, with given values */
  struct bitNode* node = malloc(sizeof(struct bitNode));
  node->bit = data;
  node->nextBit = NULL;

  /* Put new node in list */
  if(head == NULL) return node;
  else 
  {
    while(current->nextBit != NULL) current = current->nextBit;
    current->nextBit = node;
    return head;
  }
}

/**************************************************************
 * int bitAmount(struct bitNode* head)
 * 
 * Helper function that calculates the current amount of 
 * bitNodes in existance to see if a byte can be output.
 * head is the head of the bitNode list. 
 */
int bitAmount(struct bitNode* head)
{
  struct bitNode* current = head;
  int count = 0;

  while(current != NULL)
  {
    count++;
    current = current->nextBit;
  }
  return count;
}

/****************************************************************
 * struct bitNode* outputByte(struct bitNode* head, FILE* out)
 *
 * Outputs a byte from the bitNodes starting at head to the
 * output stream, out.
 */
struct bitNode* outputByte(struct bitNode* head, FILE* out)
{
  int i, output = 0;
  struct bitNode* current = head;

  /* Build the number from the given bits */
  for(i = 0; i < 8; i++)  
  {
    output += (current->bit) * pow(2, i);
    current = current->nextBit;
  }

  /* Free the used bits, set new head */
  for(i = 0; i < 8; i++)
  {
    current = head->nextBit;
    free(head);
    head = current;
  }   

  /* Output the byte */
  fputc(output, out);

  return head;
}

/************************************************************
 * void encode(FILE* in, FILE* out)
 *
 * Top level function that calls helper functions for 
 * encding. While the input steam, in, is not at the end,
 * encodes bytes and puts them to the output stream, out.
 */
void encode(FILE* in, FILE* out)
{
  int i;
  int c;
  struct bitNode* head = NULL;
  
  
  /* While not EOF */
  while(!feof(in))
  {
    c = getc(in);

    /* Add all the huffman values as bits to the list */
    for(i = 1; i < huffmanValues[c][0] + 1; i++)
    {
      head = createNodeInsert(head, huffmanValues[c][i]);
    }
  
    /* If we have enough to make a byte, do so */
    if(bitAmount(head) > 7) 
      {
	head = outputByte(head, out);
      }	
  }

  /* If the file ends but a bit isn't full, pad it with zeroes */

  if(bitAmount(head) % 8)
  {
    while(bitAmount(head) % 8) 
    {
      createNodeInsert(head, 0);
    }
    outputByte(head, out);
  }
}

/**********************************************************
 * void writeSymbolAndFreq(FILE* out)
 *
 * Writes the symbols and their respective frequencies
 * to the output steam, out.
 */
void writeSymbolAndFreq(FILE* out)
{
  int i;
  unsigned char symbol;
  unsigned long frequency;
  unsigned short symbolCount;
  
  /* Loop over gathered frequencies to output the 
     total symbols */
  for(i = 0; i < 256; i++) 
    if(frequencyMap[i] > 0) symbolCount++;

  fwrite(&symbolCount, sizeof(unsigned short), 1, out);

  /* Loop again to output each symbol and its frequency */
  for(i = 0; i < 256; i++)
  {
    if(frequencyMap[i] > 0)
    {
      symbol = i;
      frequency = frequencyMap[i];

      fwrite(&symbol, sizeof(unsigned char), 1, out);
      fwrite(&frequency, sizeof(unsigned long), 1, out);
    }
  }
}

int main(int argc, char** argv)
{
  char* infile;
  char* outfile;
  FILE* in;
  FILE* out;

  int i;
  struct QueueNode* head = NULL;
  unsigned long encodedCount;

  /* Check for valid amount of args */
  if(argc != 3) 
  {
    printf("wrong number of args\n");
    return 1;
  }
  
  infile = argv[1];
  outfile = argv[2];

  /* Open input file, check for errors */
  in = fopen(infile, "rb");
  if(in == NULL)
  {
    printf("couldn't open %s for reading\n", infile);
    return 2;
  }

  /* Open output file, check for errors */
  out = fopen(outfile, "wb");
  if(out == NULL)
  {
    printf("couldn't open %s for reading\n", outfile);
    return 3;
  }

  /* Count the frequencies of characters in the in file. */
  countFrequencies(in);
  
  /* Go to top of input file for encoding. */
  rewind(in);

  /* Make linked list of files. */
  for(i = 0; i < 256; i++) 
    if(frequencyMap[i] > 0) 
      head = createNodeLinked(head, i, frequencyMap[i]);

  /* Build huffman tree. */
  head = buildTree(head);

  /* Write symbols and frequencies, encoded, to file. */
  writeSymbolAndFreq(out);

  /* Write total amout of symbols to file. */
  encodedCount = head->frequency;
  fwrite(&encodedCount, sizeof(unsigned long), 1, out);

  /* Generate the huffman codes for each symbol. */
  generateCodesHelper(head);

  /* Print the symbol/frequency/code chart to stdout. */
  printDataValues();
  printf("Total chars = %d\n", head->frequency);

  /* Encode the input file. */
  encode(in, out);

  /* Clean up. */
  freeTree(head);
  fclose(in);
  fclose(out);

  return 0;
}
