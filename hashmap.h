#ifndef HASHMAP_H
#define HASHMAP_H

struct ranks{
        char* document_id;
        double tf_idf;
        struct ranks* next;
};

struct word{
        char* document_id;
        int wf;
        struct word* next;
};

struct llnode {
        struct word* document;
	char* word;
        int df;
        struct llnode* next;
};

struct hashmap {
        struct llnode** map;
        int num_buckets;
	int num_documents;
};

void print(struct hashmap *hm);
struct hashmap* hm_create(int num_buckets);
void hm_destroy(struct hashmap* hm);

int hash_code(struct hashmap* hm, char* word);
void hash_table_insert(struct hashmap* hm, char* word, char* document_id);

struct hashmap* training(int num_buckets);

struct hashmap* stop_word(struct hashmap* hm);

double calculate_idf(struct hashmap* hm, struct llnode* ptr);
struct hashmap* remove_stop(struct hashmap* hm, char *to_be_removed[], int length);

void read_query(char *argv, struct hashmap* hm);
double get_tf_idf(struct word* doc, double idf);

struct ranks* ranklist(char* query_word, struct hashmap* hm);

struct ranks* sort(struct ranks* rank) ;
void swap(struct ranks* a, struct ranks* b) ;
void output_to_file(struct ranks* rank);

#endif
              
