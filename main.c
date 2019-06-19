#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Ideas - brainstorming
 * Graph for entities and relations. Directed since relations are mono directional and probably weighted.
 * Someway the vertex should contain the number of its sources for each kind of relation, since the output requires not
 * the general weight of all the entity's relations, but the weight for each one.
 * This could be a map with the id of the relation as key and a structure containing entity id and weight for that specific
 * relation. Another idea could be that of doing different graphs, one for each relation. It is way less efficient with an
 * high number of relations, but by specification this shouldn't be the case.
 * Anyway at least two structures for id_ent and id_rel are needed. In order to save space, a numeric id it's better
 * and more handy for the matrix implementation. If that's the case it would be optimal to use two maps for storing ids.
 * The maps should be bidirectional, since there is the need to take name given id and id given name in O(1).
 * Noteworthy is the fact that an entity can have different relations with another one.
 */

#define PATH "../tests/suite1/batch1.1.in"

#define MAX_INPUT_LENGTH 512
#define MAX_NAME_LENGTH 128

#define BASE_ENTITIES_NUMBER 512
#define BASE_RELATIONS_NUMBER 128

#define MAX_FREED_ELEMENTS 64

typedef struct Entity Entity;
struct Entity {
    unsigned short int id;
    char name[MAX_NAME_LENGTH];
};

typedef struct Relation Relation;
struct Relation {
    unsigned short int id;
    char name[MAX_NAME_LENGTH];
};

Entity entities[BASE_ENTITIES_NUMBER];
int freed_entities[MAX_FREED_ELEMENTS] = {};
unsigned short int entities_count = 0;

Relation relations[BASE_RELATIONS_NUMBER];
int freed_relations[MAX_FREED_ELEMENTS] = {};
unsigned short int relations_count = 0;

int main() {
    // redirect file to stdin
    if (freopen(PATH, "r", stdin) == NULL) {
        printf("Error: could not redirect file to stdin\n");
        exit(1);
    }

    char line[MAX_INPUT_LENGTH];
    char *token = NULL;
    unsigned short int exit = 0;

    do {
        // read line
        fgets(line, MAX_INPUT_LENGTH, stdin);

        // parse line [space, double quotes]
        token = strtok(line, " \"");

        // enter in right case
        if (strcmp(token, "addrel") == 0) { // add relation, comes first since it should be the most common
            printf("%s ", token);
            token = strtok(NULL, " \""); // first parameter: first entity
            printf("%s ", token);
            token = strtok(NULL, " \""); // second parameter: second entity
            printf("%s ", token);
            token = strtok(NULL, " \""); // third parameter: relation
            printf("%s\n", token);
        } else if (strcmp(token, "addent") == 0) { // add entity
            printf("%s ", token);
            token = strtok(NULL, " \""); // first parameter: entity name
            printf("%s\n", token);
        } else if (strcmp(token, "report\n") == 0) { // report
            printf("%s", token);
        } else if (strcmp(token, "end\n") == 0) { // end
            printf("%s", token);
            exit = 1;
        }
    } while (exit == 0);

    return 0;
}