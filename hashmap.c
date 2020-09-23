#include "hashmap.h"
#include <stdio.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

//global linked list containing llist of ranks
struct ranks* rank = NULL;

//prints out the hashmap
void print(struct hashmap *hm)
{
	int i;
	for(i=0; i<hm->num_buckets; i++)
	{
		struct llnode* temp = hm->map[i];
		while(temp != NULL) {
			printf("bucket index: %d\n", i);
			printf("word: %s\ndf: %d\n", temp->word, temp->df);
			
			struct word* temp2 = temp->document;
			while(temp2 != NULL) {
				printf("document_id: %s\nwf: %d\n\n", temp2->document_id, temp2->wf);
				temp2 = temp2->next;
			}
		 	
			temp = temp->next;
		}
	}
}

//@returns the index of the hash bucket that a given word should be at
int hash_code(struct hashmap* hm, char* word)
{
	int index = 0;
	char* ptr = word;

	while(*ptr != '\0')
	{
		index += *ptr; //sum of all ascii values in the word
		ptr++;
	}			
	
	index %= hm->num_buckets;// index = stringValue mod num_buckets 
	return index;	
}

//create a return a hashmap
struct hashmap* hm_create(int num_buckets)
{
	struct hashmap *hm;

	hm = (struct hashmap*)malloc(sizeof(struct hashmap)); //allocate memory for hashmap
	hm->num_buckets = num_buckets;
	hm->map = (struct llnode**)malloc(num_buckets*sizeof(struct llnode*));//allocate memory for map

	int i;
	
	for(i = 0; i < num_buckets; i++)
	{
		hm->map[i] = NULL; 
	}

	return hm;	
}

//frees all mallocced memory
void hm_destroy(struct hashmap* hm)
{
	int i;
	
	for(i=0; i<hm->num_buckets; i++)
	{
		struct llnode* temp = hm->map[i];
		while(hm->map[i] != NULL)
		{
			struct word* temp2 = hm->map[i]->document;
			while(hm->map[i]->document != NULL)
			{
				hm->map[i]->document = hm->map[i]->document->next;
				free(temp2->document_id);
				free(temp2);
				temp2 = hm->map[i]->document;
			}

			hm->map[i] = hm->map[i]->next;
			free(temp->word);
			free(temp);
			temp = hm->map[i];
		}
		free(hm->map[i]);
	}

	free(hm->map);
	free(hm);

	struct ranks* rankptr = rank;
	while(rank != NULL)
	{
		rank = rank->next;
		free(rankptr->document_id);
		free(rankptr);
		rankptr = rank;
	}
	//free(rank);
}

//inserts a word into the hashtable
void hash_table_insert(struct hashmap* hm, char* word, char* document_id)
{
	int i;
	i = hash_code(hm, word); //retrieve hashed index

	struct llnode* temp;
	struct word* temp2;

	//if first node at the hashed index is empty
	if(hm->map[i] == NULL) //if there are no occurences of this word recorded yet
	{
		//create new llnode
		struct llnode* node = (struct llnode*) malloc(sizeof(struct llnode));
		node->word = (char*) malloc(sizeof(char)*20);
		strcpy(node->word, word); //set word of the node = to passed in word	
		node->df = 1; // df = 1
		node->next = NULL;
		hm->map[i] = node; 
		
		//create new word
		struct word* doc = (struct word*) malloc(sizeof(struct word));
		doc->document_id = (char*) malloc(sizeof(char)*100); 	
		strcpy(doc->document_id, document_id); //set document of the node = to passed in document_id
		doc->wf = 1; // wf = 1
		doc->next = NULL;
		hm->map[i]->document = doc;
		return;
	}	
	
	//else iterate through linked list of nodes
	temp = hm->map[i];
	temp2 = hm->map[i]->document;
	while(temp->next != NULL)
	{
		if(strcmp(word, temp->word) == 0)
		{
			temp2 = temp->document;
			while(temp2->next != NULL)
			{		
				if(strcmp(document_id, temp2->document_id) == 0)
				{
					temp2->wf += 1;
					return;
				}
				temp2 = temp2->next;
			}

			if(strcmp(document_id, temp2->document_id) == 0)
			{
				temp2->wf += 1;
				return;
			}
			
			//else, you are at end of 2nd list so make a new word with new document_id and wf = 1
			temp->df = temp->df + 1; //increment df because a new document was found
			
			struct word* doc = (struct word*) malloc(sizeof(struct word));
            doc->document_id = (char*) malloc(sizeof(char)*100);
			strcpy(doc->document_id, document_id); //set document of the node = to passed in document_id
			doc->wf = 1;
			doc->next = NULL;
			temp2->next = doc;	
			return;	
		}
		temp = temp->next; //increment pointer
	}
 
	//if the node you are currently at is = to node trying to insert
	if(strcmp(word, temp->word) == 0)
	{
			temp2 = temp->document;
			while(temp2->next != NULL)
			{		
				//if word->document_id and document_id match, update wf
				if(strcmp(document_id, temp2->document_id) == 0)
				{
					temp2->wf += 1;
					return;
				}
				temp2 = temp2->next;
			}

			if(strcmp(document_id, temp2->document_id) == 0)
			{
				temp2->wf += 1;
				return;
			}
			
			//else, you are at end of 2nd list so make a new word with new document_id and wf = 1
			temp->df = temp->df + 1; //increment df because a new document was found
			
			struct word* doc = (struct word*) malloc(sizeof(struct word));
            doc->document_id = (char*) malloc(sizeof(char)*100);
			strcpy(doc->document_id, document_id); //set document of the node = to passed in document_id
			doc->wf = 1;
			doc->next = NULL;
			temp2->next = doc;	
			return;	
	}

	//	now at end of list of nodes at index, create new node here
    struct llnode* node = (struct llnode*) malloc(sizeof(struct llnode));
    node->word = (char*) malloc(sizeof(char)*20);
    strcpy(node->word, word); //set word of the node = to passed in word    
    node->df = 1; // df = 1
	node->next = NULL;
    temp->next = node;		

	struct word* doc = (struct word*) malloc(sizeof(struct word));
	doc->document_id = (char*) malloc(sizeof(char)*100);
	strcpy(doc->document_id, document_id); //set document of the node = to passed in document_id
	doc->wf = 1; // wf = 1
	doc->next = NULL;
    temp->next->document = doc;			

}

//returns the idf of a given llnode
double calculate_idf(struct hashmap* hm, struct llnode* ptr)
{
	int N;
	double idf;
	
	N = hm->num_documents;

	if(ptr->df == 0) {
		idf =  log10((double)N/(double)(1+ptr->df));
	}		

	else {
		idf = log10((double)N/(double)(ptr->df));
	}

	return idf;
}

//removes all stop words in hm
struct hashmap* remove_stop(struct hashmap* hm, char *to_be_removed[], int length)
{
	int i,j;

	//iterate through array of words to be removed
	for(j=0; j<length; j++)
	{
		//i = index of current word to be removed
		i = hash_code(hm, to_be_removed[j]); 

		struct llnode* ptr;
		struct llnode* follow;

		//ptr = current node in hm
		ptr = hm->map[i];

		//if the word you want to remove is at the head of list, remove it
		if((strcmp(to_be_removed[j], ptr->word) == 0))
		{
			struct word* temp2 = hm->map[i]->document;
			while(hm->map[i]->document != NULL)
			{
				hm->map[i]->document = hm->map[i]->document->next;
				free(temp2->document_id);
				free(temp2);
				temp2 = hm->map[i]->document;
			}

			hm->map[i] = hm->map[i]->next;
			free(ptr->word);
			free(ptr);
			continue;
		}

		//temp = head of list
		struct llnode* temp = ptr;
		follow = NULL;

		//iterate through until find llnode to delete
		while(temp != NULL && (strcmp(to_be_removed[j], temp->word) != 0))
		{
			follow = temp;
			temp = temp->next;
		}

		//remove the node's document list
		struct word* temp2 = temp->document;
		while(temp->document != NULL)
		{
			temp2 = temp->document;
			temp->document = temp->document->next;
			free(temp2->document_id);
			free(temp2);
			temp2 = NULL;
			//temp2 = temp->document;
		}

		//delete the llnode word
		follow->next = temp->next;
		free(temp->word);
		free(temp);
		temp = NULL;
		continue;

	}

	return hm;
}

//removes a hashmap without stop words in it
struct hashmap* stop_word(struct hashmap* hm)
{
	int i, counter;
	double idf;
	struct llnode* ptr;
	char *to_be_removed[hm->num_documents]; //stores words

	//iterates through list of llnodes at each index of hashmap
    for(i=0; i<hm->num_buckets; i++)
    {
		counter = 0; //counts the num elements in to
		ptr = hm->map[i]; //current llnode
		idf = calculate_idf(hm, ptr); //idf of current llnode

		//if there is only one node in the bucket and it's a stop word
		if(ptr->next == NULL && idf == 0)
		{
			to_be_removed[counter] = ptr->word; //add node to array to be removed
			counter++;                    		
		}	
    
		//else, iterate rest of index to check for stop words
        while(ptr->next != NULL) 
		{

			idf = calculate_idf(hm, ptr); //idf of current llnode

			if(idf == 0)
			{
				to_be_removed[counter] = ptr->word;
				counter++;                    		
			}	
			ptr = ptr->next;			
		}	
		
		//if there is only one node in the bucket and it's a stop word
		if(ptr->next == NULL && idf == 0)
		{
			to_be_removed[counter] = ptr->word; //add node to array to be removed
			counter++;                    		
		}	

		hm = remove_stop(hm, to_be_removed, counter);
	}	
	
	return hm;
}

//completes the training process of creating/filling a hashtable with document contents and removing the stop words
struct hashmap* training(int num_buckets)
{
	unsigned long int retreval;
	int i;
	glob_t result;
	char inputArray[20];
	char input[20];
	struct hashmap* hm;
	FILE *dataFile;

	//create a hashmap hm with inputted number of buckets
	hm = hm_create(num_buckets);

	//if retreval = 0 files read successfully, else did not read files properly
	retreval = glob("./p5docs/*.txt", 0, NULL, &result); 

	if(retreval != 0)
	{
		printf("Files not read properly");
		return hm;	
	}

	//iterate through all inpfiles in p5docs
	while(result.gl_pathv[i])
	{
		hm->num_documents+=1;
		dataFile = fopen(result.gl_pathv[i], "r");
		
		//insert each word to hashmap in file # i
		while(fscanf(dataFile, "%s", inputArray) != EOF)
		{
			//convert each word lower case before inserting it into the hashmap
			int k = 0;
			for (k = 0; k < 20;k++){
				input[k] = tolower(inputArray[k]);
			}		
			hash_table_insert(hm, input, result.gl_pathv[i]);
		}

		//create rank list containing each document with tf-idf 's of 0
		//if list of ranks is empty, create head node
		if(i == 0)
		{
			struct ranks* ranknode = (struct ranks*) malloc(sizeof(struct ranks));
			ranknode->document_id = (char*) malloc(sizeof(char)*100);
			strcpy(ranknode->document_id, result.gl_pathv[i]); //set document of the ranknode = to passed in document_id
			ranknode->tf_idf = 0;
			ranknode->next = NULL;
			rank = ranknode;
		}

		//else, find the end of te list and add new node to last index
		else 
		{ 
			struct ranks* rankptr = rank;

			//find end of list
			while(rankptr->next != NULL)
			{
				rankptr = rankptr->next;
			}

			//if list of ranks is empty, create head node
			struct ranks* ranknode = (struct ranks*) malloc(sizeof(struct ranks));
			ranknode->document_id = (char*) malloc(sizeof(char)*100);
			strcpy(ranknode->document_id, result.gl_pathv[i]); //set document of the ranknode = to passed in document_id
			ranknode->tf_idf = 0;
			ranknode->next = NULL;
			rankptr->next = ranknode;
		}
		i++;
	}

	fclose(dataFile);

	hm = stop_word(hm); //remove stop words from the created hasmhmap
	globfree(&result);
	return hm;

}

double get_tf_idf(struct word* doc, double idf)
{
	return (doc->wf * idf);
}

//calculates sum of tf_idf score of a word across in search the query
struct ranks* ranklist(char* query_word, struct hashmap* hm)
{
	int i = hash_code(hm, query_word);
	double idf;
	double tf_idf;
	bool found = false;

	struct llnode* ptr = hm->map[i];

	//iterate through hash index that the query word is found in
	while(ptr!= NULL)
	{	
		//if the node word you are at = the query word being searched
		if((strcmp(ptr->word, query_word) == 0)) 
		{
			//calculate idf of the word
			idf = calculate_idf(hm, ptr);

			struct word* docptr = ptr->document;

			//iterate through list of documents summing their idfs
			while(docptr != NULL)
			{
				found = false;

				//get tfidf for that document/word combo
				tf_idf = get_tf_idf(docptr, idf);
				
				//check if you find the document within the rank list at middle of list
				struct ranks* rankptr = rank;
				while(rankptr->next != NULL)
				{
					//if the document exists in the ranks array, update/increment its tf-idk
					if(strcmp(docptr->document_id, rankptr->document_id) == 0)
					{
						//strcpy(rank->document_id, docptr->document_id); //update document_id
						rankptr->tf_idf += tf_idf;
						found = true;
						break;
					}
					rankptr = rankptr->next;
				}

				if(found == false)
				{
					rankptr->tf_idf += tf_idf;
				}

				docptr = docptr->next;
			}
		}
		ptr = ptr->next;
	}

	return rank;
}

//reads in search query word by word and creates a list of ranks
void read_query(char *argv, struct hashmap* hm) //, struct hashmap* hm)
{
	struct ranks* R;
	char* query_word;
	char query[20];
	struct ranks* rank;
	rank = NULL;

	query_word = strtok(argv, " ");

	//for each query search word
	while(query_word != NULL) 
	{
		//convert each word to all lower case when searching
		int j = 0;
		for (j = 0; j < 20; j++) {
			query[j] = tolower(query_word[j]);
		}

		//create a list of the ranks via tf-idfs of each document that the query word is found in
		rank = ranklist(query, hm);

		query_word = strtok(NULL, " "); //get next word in query		
	}

	R = sort(rank); //sort the ranks in dexcending order

	output_to_file(R); //output each filename/socre pair to search_scores.txt file

}

//print the dontents of the highest ranked document
void output_to_file(struct ranks* R)
{
	//printf("Document id%s\n", R->document_id);
	FILE *fptr = fopen("search_scores.txt", "w"); 
	struct ranks* ptrR2 = R;
	char c;

	//open the first file in Rank array
	FILE *fptr2 = fopen(ptrR2->document_id, "r"); 

	//read contents of file
	c = fgetc(fptr2); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(fptr2); 
    } 

	printf("\n");
  
    fclose(fptr2); 

	//go through ranked list
	while(ptrR2 != NULL)
	{
		fprintf(fptr,"<%s>:<%f>\n", ptrR2->document_id, ptrR2->tf_idf); 
		ptrR2 = ptrR2->next;
	}
}

//sorts the ranks in decreasing order
struct ranks* sort(struct ranks* rank) 
{ 
	bool swapped; 
    struct ranks* ptr; 
    struct ranks* end = NULL; 
  
    //checks if passed in list is empty
    if (rank == NULL) 
        return rank; 
  
    do
    { 
        swapped = false; 
        ptr = rank; 
		
		//iterate through ranks
        while (ptr->next != end) 
        { 
            if (ptr->tf_idf < ptr->next->tf_idf) 
            {  
                swap(ptr, ptr->next); 
                swapped = true; 
            } 
            ptr = ptr->next; 
        } 
        end = ptr; 
    } 
    while (swapped == true); 

	return rank;
} 
  
//function to swap data of two nodes a and b
void swap(struct ranks* a, struct ranks* b) 
{ 
 	struct ranks* temp = (struct ranks*) malloc(sizeof(struct ranks));
	temp->document_id = (char*) malloc(sizeof(char)*100);

	temp->tf_idf = a->tf_idf; 
	strcpy(temp->document_id, a->document_id); //set document of temp = a doc id

	a->tf_idf = b->tf_idf; 
	strcpy(a->document_id, b->document_id); 

  	b->tf_idf = temp->tf_idf;
	strcpy(b->document_id, temp->document_id); 

	free(temp->document_id);
	free(temp);
} 

int main(int argc, char *argv[])
{
	int num_buckets;
	struct hashmap* hm; 

	//end program if user enters too few arguments
	if(argc <3)
	{
		printf("ERROR, too few arguments\n");
		return 0;
	}

	//convert character num of arguments to an integer
	num_buckets = (int) *argv[1] - 48;

	//return hm with no stop words in it
	hm = training(num_buckets);

	read_query(argv[2], hm);

	hm_destroy(hm);

	return 0;
	
}
