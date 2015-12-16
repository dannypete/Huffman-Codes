/***************************************/
/* Daniel Peterson                     */
/* December 15, 2015                   */
/* CS-241 Section #3                   */
/*                                     ************************************/
/* This program uses the Huffman coding technique in order to decompress  */
/* files that have been encoded in the same way.                          */
/* For more information on the Huffman technique, see:                    */
/*     https://en.wikipedia.org/wiki/Huffman_coding                       */
/* This program expects two command-line arguments. The first is the file */
/* to be decoded. The second is the file to be created with the decoded   */
/* file results.                                                          */
/* The file will return an error for invalid command line argument amount */
/* or problems opening/closing the specified files, or etc.               */
/**************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* The longest a Huffman code can be is 127 */
#define maxHeight 127

/* Holds the Huffman Codes for each used character */
int huffmanValues[256][maxHeight];

/* The number of unique symbols in the file being decoded
   (when it's unencoded) */
int numSymbols;

/* The total number of characters in the file
   (when it's unencoded) */
int totalChars;

/* Does-It-All struct, used for linked list and tree */
struct QueueNode
{
  /* ASCII value of the character */
  int data;

  /* Frequency of the character */
  int frequency;

  /* Left child pointer */
  struct QueueNode* left;

  /* Right child pointer */
  struct QueueNode* right;

  /* Next node in list pointer */
  struct QueueNode* next;
};


/***********************************************************************************
 * struct QueueNode* insertSorted(struct QueunNode* head, struct QueueNode* newNode)
 *
 * The function inserts a node, newNode, into the list/tree hybrid pointed to
 * by head. It returns the (possibly new) head.
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
 * The function creates a new linked list node with character denoted by  data,
 * and frequency denoted by frequency. It then calls another function to insert it
 * appropriately in the list pointed to by head. It returns the (possibly new) head.
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

/********************************************************************************
 * struct QueueNode* createTreeNode(struct QueueNode* head)
 *
 * This function is called repeatedly to build the tree from existing nodes in
 * a linked list. It takes the smallest two nodes, combines them with a parent
 * node, and then reinserts the parent node back into the list/tree hybrid
 * pointed to by head. It returns the (possibly new) head.
 */
struct QueueNode* createNodeTree(struct QueueNode* head)
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

/*******************************************************
 * struct QueueNode* buildTree(struct QueueNode* head)
 *
 * This function is a helper function which calls
 * createNodeTree until the Huffman tree is fully
 * built. It creates the tree from the list/tree
 * hybrid pointed to by head, and returns the root
 * of the resulting tree.
 */
struct QueueNode* buildTree(struct QueueNode* head)
{
  while(head->next != NULL)
    head = createNodeTree(head);

  return head;
}

/***********************************************
 * void freeTree(struct QueueNode* head)
 *
 * This function frees the malloc-ed memory
 * used by the nodes which start at head.
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

/****************************************************************
 * void traverseTree(struct QueueNode* head, FILE* in, FILE* out)
 *
 * The function traverses the existing Huffman tree pointed to
 * by head. It reads in bytes from the file pointed to at in
 * and when a node is reached by going left (bit is 0) or right
 * (bit is 1), writes that character to the file pointed to by
 * out.
 */
void traverseTree(struct QueueNode* head, FILE* in, FILE* out)
{
  unsigned char rawInput;
  int bytes[8];
  struct QueueNode* current = head;
  int i = 0;
  int charCount = 0;

  while(fread(&rawInput, sizeof(unsigned char), 1, in) == 1
	&& charCount < totalChars)
  {
    for(i = 0; i < 8; i++)
    {
      bytes[i] = rawInput % 2;
      rawInput /= 2;
    }

    for(i = 0; i < 8; i++)
    {
      if(bytes[i] == 1 && current->right != NULL)
	current = current->right;

      else if(bytes[i] == 0 && current->left != NULL)
	current = current->left;

      if(current->left == NULL && current->right == NULL && charCount < totalChars)
      {
	char toWrite = current->data;
	fwrite(&toWrite, sizeof(char), 1, out);
	charCount++;
	current = head;
      }
    }
  }
}

int main(int argc, char** argv)
{
  char* infile;
  char* outfile;
  FILE* in;
  FILE* out;

  struct QueueNode* head = NULL;

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
    printf("couldn't open %s for writing\n", outfile);
    return 3;
  }

  /* Read the number of unique symbols number from the file */
  fread(&numSymbols, sizeof(unsigned short), 1, in);

  /* Read until we have correct amount of symbols and their
     frequencies */
  while(numSymbols)
  {
    int data, frequency;

    fread(&data, sizeof(unsigned char), 1, in);
    fread(&frequency, sizeof(unsigned long), 1, in);

    /* Build up the linked list as we go with the stored values */
    head = createNodeLinked(head, data, frequency);

    numSymbols--;
  }

  /* Read the total number of characters number from the file */
  fread(&totalChars, sizeof(unsigned long), 1, in);

  /* Build the Huffman tree from the existing linked list */
  head = buildTree(head);

  /* Decode the file by traversing the Huffman tree */
  traverseTree(head, in, out);

  /* Clean up */
  freeTree(head);
  fclose(in);
  fclose(out);

  return 0;

}
