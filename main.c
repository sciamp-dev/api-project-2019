#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 128
#define MAX_NAME_LENGTH 32
#define MAX_ENTITIES_NUMBER 512
#define MAX_RELATIONS_NUMBER 4
#define MAX_OUTPUT_ENTITIES 32

typedef struct Node Node;
struct Node {
    char key[MAX_NAME_LENGTH];
    short int value;
    Node *next;
};

typedef struct Map HashMap;
struct Map {
    Node **list;
};

HashMap *hashMap;

void init_hash_map() {
    hashMap = (HashMap*)malloc(sizeof(HashMap));
    hashMap->list = (Node**)malloc(sizeof(Node) * MAX_ENTITIES_NUMBER);
}

int hash_code(char *key) {
    unsigned long int value = 0;
    int hashCode = 0;

    unsigned long int i = 0;
    unsigned long int shift = 8;
    for (i = 0; i < strlen(key); i++) {
        value <<= shift;
        value += key[i];
    }

    hashCode = value % MAX_ENTITIES_NUMBER;

    return hashCode;
}

void insert(char key[], short int value) {
    int position = hash_code(key);
    Node *list = hashMap->list[position];
    Node *newNode = (Node*)malloc(sizeof(Node));
    Node *temp = list;

    while (temp) {
        if (strcmp(temp->key, key) == 0) {
            temp->value = value;
            return;
        }

        temp = temp->next;
    }

    strcpy(newNode->key, key);
    newNode->value = value;
    newNode->next = list;
    hashMap->list[position] = newNode;
}

short int lookup(char key[]) {
    int position = hash_code(key);
    Node *list = hashMap->list[position];
    Node *temp = list;

    while (temp) {
        if (strcmp(temp->key, key) == 0) {
            return temp->value;
        }

        temp = temp->next;
    }

    return -1000;
}

typedef struct EntityName EntityName;
struct EntityName {
    char name[MAX_NAME_LENGTH];
};

typedef struct Entity Entity;
struct Entity {
    char name[MAX_NAME_LENGTH];
    int count[MAX_RELATIONS_NUMBER];
};

typedef struct List List;
struct List {
    int value;
    List *next;
};

List* add_to_list(List *list, int value) {
    List *temp = list;

    if (list == NULL) {
        list = (List*)malloc(sizeof(List));
        list->value = value;
        list->next = NULL;
        return list;
    }

    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = (List*)malloc(sizeof(List));
    temp->next->value = value;
    temp->next->next = NULL;

    return list;
}

void delete_from_list(List **list, int value) {
    List *temp = *list;

    if (temp->value == value) {
        if ((*list)->next != NULL) {
            *list = (*list)->next;
            free(temp);
            return;
        }

        free(*list);
        *list = NULL;
        return;
    }

    while (temp->next != NULL) {
        if (temp->next->value == value) {
            List *to_free = temp->next;

            temp->next = to_free->next;

            free(to_free);

            return;
        }

        temp = temp->next;
    }
}

typedef struct Relation Relation;
struct Relation {
    char name[MAX_NAME_LENGTH];
    int adj_mtr[MAX_ENTITIES_NUMBER][MAX_ENTITIES_NUMBER];
    List *entities;
    EntityName entities_names[MAX_OUTPUT_ENTITIES];
    int max;
    int count_entities;
    int recalculate_max;
};

Entity entities[MAX_ENTITIES_NUMBER];
short int entities_count = 0;

Relation relations[MAX_RELATIONS_NUMBER];
int relations_count = 0;
int relations_order[MAX_RELATIONS_NUMBER];

int compare(const void *a, const void *b) {
    return strcmp((*(EntityName*)a).name, (*(EntityName*)b).name);
}

int main() {
    init_hash_map();

    // global variables initialization
    relations_count = 0;
    entities_count = 0;

    // reading and main loop
    char line[MAX_LINE_LENGTH];
    char *token = NULL;
    unsigned short int exit = 0;

    // indexes
    short int first_entity_pos;
    short int second_entity_pos;
    int relation_position;
    int new_entity_index;

    // temp
    char temp_entity[MAX_NAME_LENGTH];
    int temp_position;
    int temp_sum;

    // loops
    int i;
    int j;
    int k;
    int t;

    // counts
    int count_occurrences;
    int count_output = -1;

    // booleans
    char check;

    do {
        // read line
        fgets(line, MAX_LINE_LENGTH, stdin);

        // parse line [space, double quotes]
        token = strtok(line, " \"");

        if (memcmp(token, "addrel", 7) == 0) { // add relation
            // first parameter: first entity
            token = strtok(NULL, " \"");

            first_entity_pos = lookup(token);
            // check if first entity exists
            if (first_entity_pos > -1) {
                // second parameter: second entity
                token = strtok(NULL, " \"");

                second_entity_pos = lookup(token);
                // check if second entity exists
                if (second_entity_pos > -1) {
                    // third parameter: relation
                    token = strtok(NULL, " \"");

                    // search relation position
                    relation_position = -1;
                    for (i = 0; i < relations_count; i++) {
                        if (memcmp(token, relations[i].name, strlen(token)) == 0) {
                            relation_position = i;
                            break;
                        }
                    }

                    // if relation doesn't exists add it
                    if (relation_position == -1) {
                        relation_position = relations_count;

                        // initialize new relation
                        memcpy(relations[relation_position].name, token, MAX_NAME_LENGTH);
                        relations[relation_position].max = 0;
                        relations[relation_position].count_entities = 0;
                        relations[relation_position].recalculate_max = 0;

                        relations_count++;
                    }

                    // if relation between elements wasn't already set
                    if (relations[relation_position].adj_mtr[first_entity_pos][second_entity_pos] != 1) {
                        // set relation
                        relations[relation_position].adj_mtr[first_entity_pos][second_entity_pos] = 1;
                        entities[second_entity_pos].count[relation_position]++;
                        check = 'n';

                        // special case: first addrel for this relation
                        if (relations[relation_position].count_entities == 0) {
                            relations[relation_position].entities =
                                    add_to_list(relations[relation_position].entities, second_entity_pos);
                            relations[relation_position].max = 1;
                            relations[relation_position].count_entities = 1;
                            check = 'y';
                        }

                        // if second entity was one of the max, now it's the only max equals to max++
                        if (check == 'n') {
                            List *temp = relations[relation_position].entities;

                            // there is a on purpose memory leak here
                            while (temp != NULL) {
                                if (temp->value == second_entity_pos) {
                                    relations[relation_position].entities->value = second_entity_pos;
                                    relations[relation_position].entities->next = NULL;
                                    relations[relation_position].max++;
                                    relations[relation_position].count_entities = 1;
                                    check = 'y';
                                    break;
                                }
                                temp = temp->next;
                            }
                        }

                        // second entity may be a new max
                        if (check == 'n') {
                            count_occurrences = 0;
                            for (i = 0; i < entities_count; i++) {
                                if (relations[relation_position].adj_mtr[i][second_entity_pos] == 1) {
                                    count_occurrences++;

                                    // if count 'y' of column second_entity_pos is equal to current max, then add it as a max
                                    if (count_occurrences == relations[relation_position].max) {
                                        relations[relation_position].entities =
                                                add_to_list(relations[relation_position].entities, second_entity_pos);
                                        relations[relation_position].count_entities ++;

                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (memcmp(token, "addent", 7) == 0) { // add entity
            // first parameter: entity name
            token = strtok(NULL, " \"");

            first_entity_pos = lookup(token);
            if (first_entity_pos < 0) {
                // check if entity never existed
                if (first_entity_pos == -1000) {
                    // add new entity
                    memcpy(entities[entities_count].name, token, MAX_NAME_LENGTH);
                    entities[entities_count].count[0] = 0;
                    entities[entities_count].count[0] = 0;
                    entities[entities_count].count[0] = 0;
                    entities[entities_count].count[0] = 0;

                    insert(token, entities_count);
                    entities_count++;
                } else {
                    // restore previous key
                    first_entity_pos *= -1;
                    first_entity_pos--;
                    entities[first_entity_pos].count[0] = 0;
                    entities[first_entity_pos].count[0] = 0;
                    entities[first_entity_pos].count[0] = 0;
                    entities[first_entity_pos].count[0] = 0;

                    insert(token, first_entity_pos);
                }
            }
        } else if (memcmp(token, "delrel", 7) == 0) { // del relation
            // first parameter: first entity
            token = strtok(NULL, " \"");

            first_entity_pos = lookup(token);
            // check if first entity exists
            if (first_entity_pos > -1) {
                // second parameter: second entity
                token = strtok(NULL, " \"");

                second_entity_pos = lookup(token);
                // check if second entity exists
                if (second_entity_pos > -1) {
                    // third parameter: relation
                    token = strtok(NULL, " \"");

                    // try to find relation position
                    relation_position = -1;
                    for (i = 0; i < relations_count; i++) {
                        if (strcmp(relations[i].name, token) == 0) {
                            relation_position = i;
                            break;
                        }
                    }

                    // if relation exists we can proceed
                    if (relation_position != -1) {
                        // do only if relation was set
                        if (relations[relation_position].adj_mtr[first_entity_pos][second_entity_pos] == 1)  {
                            // unset relation
                            relations[relation_position].adj_mtr[first_entity_pos][second_entity_pos] = 0;
                            entities[second_entity_pos].count[relation_position]--;

                            List *temp = relations[relation_position].entities;

                            while (temp != NULL) {
                                // check if second_entity_pos is between the maxs
                                if (temp->value == second_entity_pos) {
                                    // check if this is the only max
                                    if (relations[relation_position].count_entities == 1) {
                                        // flag relation for recalculation
                                        relations[relation_position].recalculate_max = 1;
                                    } else {
                                        // remove from current maxs
                                        delete_from_list(&relations[relation_position].entities, second_entity_pos);
                                        relations[relation_position].count_entities--;
                                    }
                                    break;
                                }
                                temp = temp->next;
                            }
                        }
                    }
                }
            }
        } else if (memcmp(token, "delent", 7) == 0) { // del entity
            // first parameter: entity name
            token = strtok(NULL, " \"");

            first_entity_pos = lookup(token);
            // check if entity exists before proceeding
            if (first_entity_pos > -1) {
                // set column of first_entity_pos to -1
                for (j = 0; j < relations_count; j++) {
                    for (i = 0; i < entities_count; i++) {
                        relations[j].adj_mtr[i][first_entity_pos] = -1;
                    }
                    // reset entity in-rel count
                    entities[first_entity_pos].count[j] = 0;

                    // check if first_entity_pos is between maxs for any relation
                    List *temp  = relations[j].entities;

                    while (temp != NULL) {
                        if (temp->value == first_entity_pos) {
                            // check if it's the only one
                            if (relations[j].count_entities == 1) {
                                // signal to recalculate maxs
                                relations[j].recalculate_max = 1;
                            } else {
                                // remove from current maxs
                                delete_from_list(&relations[j].entities, first_entity_pos);
                                relations[j].count_entities--;
                            }

                            break;
                        }
                        temp = temp->next;
                    }
                }

                // set row to 'x' and check if it changes any max
                for (j = 0; j < relations_count; j++) {
                    for (t = 0; t < entities_count; t++) {
                        // proceed only if relation was set
                        if (relations[j].adj_mtr[first_entity_pos][t] == 1) {
                            relations[j].adj_mtr[first_entity_pos][t] = -1;
                            entities[t].count[j]--;

                            // check if t is between maxs for any relation
                            List *temp = relations[j].entities;

                            while (temp != NULL) {
                                if (temp->value == t) {
                                    // check if it's the only one
                                    if (relations[j].count_entities == 1) {
                                        // signal to recalculate maxs
                                        relations[j].recalculate_max = 1;
                                    } else {
                                        // remove from current max
                                        delete_from_list(&relations[j].entities, t);
                                        relations[j].count_entities--;
                                    }

                                    break;
                                }
                                temp = temp->next;
                            }
                        }
                    }
                }

                insert(token, (first_entity_pos * -1) - 1);
            }
        } else if (memcmp(token, "report\n", 8) == 0) { // report
            // if no relations output is "none"
            if (relations_count == 0) {
                printf("none\n");
            } else {
                // prepare relations order array
                relations_order[0] = 0;
                relations_order[1] = 1;
                relations_order[2] = 2;
                relations_order[3] = 3;

                // sort relations using relations order array
                for (i = 0; i < relations_count - 1; i++) {
                    for (j = 0; j < relations_count - i - 1; j++) {
                        if (strcmp(relations[j].name, relations[j + 1].name) > 0) {
                            temp_position = relations_order[j];
                            relations_order[j] = relations_order[j + 1];
                            relations_order[j + 1] = temp_position;
                        }
                    }
                }

                // check if any relation signaled to recalculate maxs
                for (i = 0; i < relations_count; i++) {
                    if (relations[i].recalculate_max == 1) {
                        // on purpose memory leak (free() it's slow)
                        relations[i].entities = NULL;

                        for (k = 0; k < entities_count; k++) {
                            if (entities[k].count[i] > relations[i].max) {
                                relations[i].max = entities[k].count[i];
                            }
                        }

                        // Recalculate maxs
                        check = 'n';
                        do {
                            relations[i].count_entities = 0;

                            for (k = 0; k < entities_count && relations[i].count_entities < MAX_OUTPUT_ENTITIES; k++) {
                                if (entities[k].count[i] > relations[i].max) {
                                    relations[i].max = entities[k].count[i];
                                }
                                if (entities[k].count[i] == relations[i].max) {
                                    check = 'y';

                                    relations[i].entities = add_to_list(relations[i].entities, k);
                                    relations[i].count_entities++;
                                }
                            }

                            if (check == 'n') {
                                relations[i].max--;
                            }
                        } while(check == 'n' && relations[i].max != 0);

                        // Reset
                        relations[i].recalculate_max = 0;
                    }
                }

                for (i = 0; i < relations_count; i++) {
                    relation_position = relations_order[i];

                    // fill names from indexes
                    List *temp = relations[relation_position].entities;
                    j = 0;
                    while (temp != NULL) {
                        strcpy(relations[relation_position].entities_names[j].name, entities[temp->value].name);
                        temp = temp->next;
                        j++;
                    }

                    // sort
                    qsort(&relations[relation_position].entities_names,
                          relations[relation_position].count_entities,
                          sizeof(EntityName),
                          compare);

                    // print
                    if (relations[relation_position].max > 0) {
                        fputs("\"", stdout);
                        fputs(relations[relation_position].name, stdout);
                        fputs("\" ", stdout);
                        for (j = 0; j < relations[relation_position].count_entities; j++) {
                            fputs("\"", stdout);
                            fputs(relations[relation_position].entities_names[j].name, stdout);
                            fputs("\" ", stdout);
                        }
                        printf("%d; ", relations[relation_position].max);
                    }
                }

                printf("\n");
            }
        } else if (memcmp(token, "end\n", 5) == 0) { // end
            exit = 1;
        }
    } while(exit == 0);

    return 0;
}