# Final test of algorithm and data structure

## Problem
Social network like structure. There are named entities and named relations. Relations between entities are mono directional.

### Input
- **addent "id_ent"**
<br>Adds an entity to the list of entities, if it already exists it doesn't do anything
- **delent "id_ent"**
<br>Deletes the corresponding identity and all the associated relations (doesn't matter if it's source or destination)
- **addrel "id_orig" "id_dest" "id_rel"**
<br>- Adds a relation named id_rel between id_orig (source) and id_dest (destination)
<br>- If any of the parameters doesn't exist it does nothing
<br>- If the relation is new, it's added to the list of relations
- **delrel "id_orig" "id_dest" "id_rel"**
<br>Deletes the relation between the given entities, if it doesn't exist it does nothing
- **report**
<br>Print a detailed report of the current state
- **end**
<br>End of the sequence of commands, closes the application

### Output
The output must contain the list of the relations, giving for each one the entity which has the highest number of sources

### Example
| Input  | Output |
| ------------- | ------------- |
| addent "alice" |  |
| addent "bruno" |  |
| addent "carlo" |  |
| addent "dario" |  |
| report | none |
| addrel "carlo" "bruno" "amico_di" |  |
| report | "amico_di" "bruno" 1; |
| addrel "carlo" "alice" "amico_di" |  |
| report | "amico_di" "alice" "bruno" 1; |
| addrel "alice" "bruno" "amico_di" |  |
| report | "amico_di" "bruno" 2; |
| addrel "bruno" "dario" "compagno_di" |  |
| report | "amico_di" "bruno" 2; "compagno_di" "dario" 1; |
| delrel "carlo" "alice" "amico_di" |  |
| report | "amico_di" "bruno" 2; "compagno_di" "dario" 1; |
| addrel "carlo" "alice" "compagno_di" |  |
| report | "amico_di" "bruno" 2; "compagno_di" "alice" "dario" 1; |
| addrel "carlo" "bruno" "compagno_di" |  |
| report | "amico_di" "bruno" 2; "compagno_di" "alice" "bruno" "dario" 1; |
| delent "alice" |  |
| report | "amico_di" "bruno" 1; "compagno_di" "bruno" "dario" 1; |
| end |  |

### Assumptions
- Names of entities and relations are case sensitive and can contain only numbers, letters and the symbols - and _
- Names of entities and relations are always between double quotes (e.g. "name")
- Commands and parameter are divided by spaces
- The command *end* doesn't print an output
- Names are ordered by ASCII representation

### Use
./api-project-2019 < path_to_input_file