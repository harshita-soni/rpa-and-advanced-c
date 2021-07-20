/* Program to evaluate candidate routines for Robotic Process Automation.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2020, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton
  file, the  program contained in this submission is completely my own
  individual work, except where explicitly noted by further comments that
  provide details otherwise.  I understand that work that has been developed
  by another student, or by me in collaboration with other students, or by
  non-students as a result of request, solicitation, or payment, may not be
  submitted for assessment in this subject.  I understand that submitting for
  assessment work developed by or in collaboration with other students or
  non-students constitutes Academic Misconduct, and may be penalized by mark
  deductions, or by other penalties determined via the University of
  Melbourne Academic Honesty Policy, as described at
  https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my
  work to other students, regardless of my intention or any undertakings made
  to me by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment
  specification to any form of code authoring or assignment tutoring service,
  or drawing the attention of others to such services and code that may have
  been made available via such a service, may be regarded as Student General
  Misconduct (interfering with the teaching activities of the University
  and/or inciting others to commit Academic Misconduct).  I understand that
  an allegation of Student General Misconduct may arise regardless of whether
  or not I personally make use of such solutions or sought benefit from such
  actions.

   Signed by: Harshita Soni   Student ID: 1138784
   Dated:     21 October 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

/* #define's -----------------------------------------------------------------*/

#define ASIZE 26
#define CH_CR       '\r'   // CR character in DOS-format files
#define CH_NL       '\n'   // newline character
#define CH_HASH      '#'   // separator used in input files
#define CH_COLON     ':'   // separator used in input action quintuples
#define TRUE          1    // true state of variables
#define FALSE         0    // false state of variables
#define UNASSIGNED    2    // variables that haven't been set to true or false
#define QUINTUPLE     5    // number of elements in input action quintuple
#define PRECON_TRUE   0
#define PRECON_FALSE  1
#define ACTION_NAME   2
#define EFFECT_TRUE   3
#define EFFECT_FALSE  4
#define CH_A_SMALL   'a'
#define CH_A_BIG     'A'

/* type definitions ----------------------------------------------------------*/

// state (values of the 26 Boolean variables)
typedef unsigned char state_t[ASIZE];

// action
typedef struct action action_t;
struct action {
    char name;        // action name
    state_t precon;   // precondition
    state_t effect;   // effect
};

// step in a trace
typedef struct step step_t;
struct step {
    action_t *action; // pointer to an action performed at this step
    step_t   *next;   // pointer to the next step in this trace
};

// trace (implemented as a linked list)
typedef struct {
    step_t *head;     // pointer to the step in the head of the trace
    step_t *tail;     // pointer to the step in the tail of the trace
} trace_t;

/* function prototypes -------------------------------------------------------*/
trace_t* make_empty_trace(void);
trace_t* insert_at_tail(trace_t*, action_t*);
void free_trace(trace_t*);

int mygetchar();
void handle_commands(trace_t *R);
int read_stage1and2(trace_t R, int c, state_t curr, action_t all_actions[],
                     int stage_num, int advanced);
void read_stage0(int c, action_t *a, int action_quintuple,
                 action_t all_actions[]);
void read_action(action_t *a, int action_quintuple, int c);
void read_initial_state(state_t initial, int c);
int check_trace_validity(trace_t *R, state_t initial, int len,
                         int num_distinct);
void print_stage0(trace_t *R, state_t initial, int valid, int len,
                  int num_distinct, int limit);
void find_similar_traces2(trace_t *R, state_t required, int advanced);
void find_similar_traces1(trace_t *R, state_t required);
void get_preconditions(trace_t *R, state_t subset_precons, step_t *start,
                       step_t *curr);
void handle_stage2(state_t required, state_t subset_effect,
                   state_t subset_precons);
void print_state(state_t arr);
void apply_effect(action_t *ac, state_t curr);

/* where it all happens ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    /* ALGORITHMS ARE FUN!! */

    trace_t *R = make_empty_trace();
    // the following function reads stdin input and calls necessary functions
    // to execute the required actions

    handle_commands(R);
    //handle_commands(R) also frees the trace once all work is done!

    return EXIT_SUCCESS;        // we are done!!! algorithms are fun!!!
}

/* function definitions ------------------------------------------------------*/

// Adapted version of the make_empty_list function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
trace_t
*make_empty_trace(void) {
    trace_t *R;
    R = (trace_t*)malloc(sizeof(*R));
    assert(R!=NULL);
    R->head = R->tail = NULL;
    return R;
}

// Adapted version of the insert_at_foot function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
trace_t
*insert_at_tail(trace_t* R, action_t* addr) {
    step_t *new;
    new = (step_t*)malloc(sizeof(*new));
    assert(R!=NULL && new!=NULL);
    new->action = addr;
    new->next = NULL;
    if (R->tail==NULL) { /* this is the first insertion into the trace */
        R->head = R->tail = new;
    } else {
        R->tail->next = new;
        R->tail = new;
    }
    return R;
}

// Adapted version of the free_list function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
void
free_trace(trace_t* R) {
    step_t *curr, *prev;
    assert(R!=NULL);
    curr = R->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(R);
}

int mygetchar() {
    // read one character at a time from stdin, omitting the carriage return
    int c;
    while ((c=getchar())=='\r') {
    }
    return c;
}

void handle_commands(trace_t *R) {
    // reads input and processes it to carry out required functions
    int c, i, valid, action_quintuple = 0, stage_num, advanced;
    int len_of_trace = 0, distinct_actions = 0;  //initialising to zero

    // array to store the initial states
    state_t initial = {FALSE}, curr;

    // a: action_t pointer to read and store actions
    // all_actions: an array of 26 action_t structs, used to store distinct
    //
    action_t *a, all_actions[ASIZE];
    for (i = 0; i<ASIZE; i++) {
        all_actions[i].name = FALSE;  // initialise all action names to false
    }

    // the action buffer is malloced and variables are set to unassigned
    a = (action_t*)malloc(sizeof(*a));
    memset(a->precon, UNASSIGNED, ASIZE);
    memset(a->effect, UNASSIGNED, ASIZE);

    // read initial state
    while ((c=mygetchar())!=EOF && c!=CH_HASH) {
        read_initial_state(initial, c);
    }

    // read stage 0 input
    read_stage0(c, a, action_quintuple, all_actions);

    // now that all actions are read, count distinct actions
    for (i = 0; i<ASIZE; i++) {
        if (isalpha(all_actions[i].name)) {
            distinct_actions += 1;
        }
    }
    mygetchar();    // consume newline that follows

    // next read one char at a time and insert into the trace linked list
    while ((c=mygetchar())!=EOF && c!=CH_NL) {
        len_of_trace += 1;
        insert_at_tail(R, &all_actions[c - 'A']);
    }
    // now validate and print accordingly as per the requirements of stage 0
    valid = check_trace_validity(R, initial, len_of_trace, distinct_actions);
    if (!valid) {
        return;
    }
    /* -----end of stage 0 input, printing and functions-----  */

    // if further input available and trace is valid, continue
    if ((c=mygetchar()) == CH_HASH && valid) {
        stage_num = 1; advanced = FALSE;
        c = read_stage1and2(*R, c, curr, all_actions, stage_num, advanced);
    }

    /* -----end of stage 1 input, printing and functions-----  */

    // if further input available and trace is valid, continue
    if (c!=EOF && c == CH_HASH && valid) {
        stage_num = 2; advanced = TRUE;
        read_stage1and2(*R, c, curr, all_actions, stage_num, advanced);
        printf("==THE END===============================\n");
    }
    /* -----end of stage 2 input, printing and functions-----  */

    // be a good programmer and free the malloced action buffer
    free(a);
    a = NULL;
    free(R);
    R = NULL;
}

int read_stage1and2(trace_t R, int c, state_t curr, action_t all_actions[],
                     int stage_num, int advanced) {
    int first_char=TRUE, first_routine = TRUE;
    memset(curr, UNASSIGNED, ASIZE);

    mygetchar();  // consume newline
    while ((c=mygetchar())!=EOF && c!=CH_HASH) {
        if (c == CH_NL) {
            printf("%c", (char)c);
            if (stage_num==1) find_similar_traces1(&R, curr);
            else if (stage_num==2) find_similar_traces2(&R, curr, advanced);
            memset(curr, UNASSIGNED, ASIZE);
            first_char=TRUE;
        }
        if (isupper(c)) {
            if (first_routine) {
                printf("==STAGE %d===============================\n", stage_num);
            } else if (first_char && !first_routine) {
                printf("----------------------------------------\n");
            }
            first_routine = FALSE;
            if (first_char) printf("Candidate routine: ");
            printf("%c", (char)c);
            apply_effect(&all_actions[c - CH_A_BIG], curr);
            first_char=FALSE;
        }
    }
    return c;
}

void get_preconditions(trace_t *R, state_t subset_precons, step_t *start,
                       step_t *curr) {
    // gets the cumulative preconditions of a sub sequence in a state_t array
    int i;
    memset(subset_precons, UNASSIGNED, ASIZE);
    step_t *from=start, *to=curr;
    assert(R!=NULL);

    while(from && from!=to) {
        for (i = 0; i<ASIZE; i++) {
            // store precons of variables who have previously not been assigned
            if (from->action->precon[i] != 2 && subset_precons[i] == 2) {
                subset_precons[i] = from->action->precon[i];
            }
        }
        from = from->next;
    }
    // get the last action in subsequence
    for (i = 0; i<ASIZE; i++) {
        if (from->action->precon[i] != 2 && subset_precons[i] == 2) {
            subset_precons[i] = from->action->precon[i];
        }
    }
}

void read_stage0(int c, action_t *a, int action_quintuple,
                 action_t all_actions[]) {
    int index;
    while ((c = mygetchar()) != EOF && c != CH_HASH) {
        if (c == CH_NL) {
            index = (int) a->name - (int) 'A';
            if (index >= 0 && action_quintuple == 4) {
                all_actions[index].name = a->name;
                memcpy(all_actions[index].precon, a->precon,
                       ASIZE * sizeof(unsigned char));
                memcpy(all_actions[index].effect, a->effect,
                       ASIZE * sizeof(unsigned char));
                action_quintuple = 0;
                memset(a->precon, 2, ASIZE);
                memset(a->effect, 2, ASIZE);
            }
        }
        if (c == CH_COLON) {
            action_quintuple += 1;
        }
        if (isalpha(c)) {
            read_action(a, action_quintuple, c);
        }
    }
}

void read_action(action_t *a, int action_quintuple, int c) {
    // layout of an action is -->  ab:c:X:cf:b
    // first ':' marks end of true variables in precon,
    // second ':' marks end of false variables in precon,
    // third ':' marks end of action name,
    // fourth ':' marks end of true variable in effect
    // after the fourth ':' we have the false variables in the effect

    // action quintuple is an int which tells us where we are in the action
    if (islower(c) && action_quintuple == PRECON_TRUE) {
        a->precon[c-CH_A_SMALL] = TRUE;
    }
    if (islower(c) && action_quintuple == PRECON_FALSE) {
        a->precon[c-CH_A_SMALL] = FALSE;
    }
    if (isupper(c) && action_quintuple == ACTION_NAME) {
        a->name = (char)c;
    }
    if (islower(c) && action_quintuple == EFFECT_TRUE) {
        a->effect[c-CH_A_SMALL] = TRUE;
    }
    if (islower(c) && action_quintuple == EFFECT_FALSE) {
        a->effect[c-CH_A_SMALL] = FALSE;
    }
}

void read_initial_state(state_t initial, int c) {
    if (isalpha(c)) {
        initial[c - CH_A_SMALL] = 1;
    }
}

int check_trace_validity(trace_t *R, state_t initial, int len,
                         int num_distinct) {
    // checks trace validity based on the preconditions and effects of actions
    int i, valid=FALSE, diff=FALSE, limit=0;
    state_t curr;
    memcpy(curr, initial, ASIZE*sizeof(unsigned char));

    step_t *temp;
    assert(R!=NULL);

    temp = R->head;
    while (temp) {
        for (i = 0; i<ASIZE; i++) {
            if (temp->action->precon[i] != UNASSIGNED) {
                diff = (curr[i] != temp->action->precon[i]);
                if (diff) {
                    break;
                }
            }
        }
        if (!diff) {
            apply_effect(temp->action, curr);
            valid = TRUE;
        } else {
            valid = FALSE;
        }
        if (valid) {
            limit += 1;
        } else {
            print_stage0(R, initial, valid, len, num_distinct, limit);
            break;
        }
        temp = temp->next;
    }

    if (valid) {
        print_stage0(R, initial, valid, len, num_distinct, limit);
    }
    return valid;
}

void print_stage0(trace_t *R, state_t initial, int valid, int len,
                  int num_distinct, int limit) {
    state_t curr;
    memcpy(curr, initial, ASIZE*sizeof(unsigned char));

    printf("==STAGE 0===============================\n");
    printf("Number of distinct actions: %d\n", num_distinct);
    printf("Length of the input trace: %d\n", len);
    printf("Trace status: ");
    if (valid) printf("valid\n");
    else printf("invalid\n");
    printf("----------------------------------------\n");
    printf("  abcdefghijklmnopqrstuvwxyz\n");
    printf("> ");
    print_state(initial);
    step_t *temp;

    assert(R!=NULL);

    temp = R->head;
    while (temp && limit) {
        apply_effect(temp->action, curr);
        printf("%c ", temp->action->name);
        print_state(curr);
        limit -= 1;
        temp = temp->next;
    }
    free(temp);
    temp=NULL;
}

void find_similar_traces2(trace_t *R, state_t required, int advanced) {
    int i=0, skip = FALSE;
    state_t subset_effect, subset_precons;
    assert(R!=NULL);

    step_t *curr, *start = R->head, *from;
    while (start) {
        memset(subset_effect, UNASSIGNED, ASIZE);
        curr = start;
        while (curr) {
            get_preconditions(R, subset_precons, start, curr);
            handle_stage2(required, subset_effect, subset_precons);

            if (memcmp(required, subset_effect, ASIZE)==0) {
                skip = TRUE;
                printf("%5d: ", i);
                from = start;
                while (from!=curr) {
                    printf("%c", from->action->name);
                    from = from->next;
                    i+=1;
                }
                printf("\n");
                // match found, start looking from the next overlapping action
                break;
            } else {
                skip = FALSE;
                apply_effect(curr->action, subset_effect);
                get_preconditions(R, subset_precons, start, curr);
                handle_stage2(required, subset_effect, subset_precons);
                curr = curr->next;
            }
        }
        if (skip) {
            start = curr;
        }
        else {
            start = start->next;
        }
    }

    // check and print for the last action sequence in trace
    if (memcmp(required, subset_effect, ASIZE)==0) {
        printf("%5d: ", i);
        int j=0;
        from = R->head;
        while (from) {
            if(j>=i) printf("%c", from->action->name);
            from = from->next;
            j+=1;
        }
        printf("\n");
    }
}

void find_similar_traces1(trace_t *R, state_t required) {
    int j, i=0;
    state_t subset_effect;
    memset(subset_effect, UNASSIGNED, ASIZE);
    assert(R!=NULL);

    step_t *curr = R->head, *start = R->head;
    while (start && curr) {
        for (j = 0; j < ASIZE; j++) {
            // action should't alter variables that are not set by the routine
            if (required[j] == UNASSIGNED && (subset_effect[j] != UNASSIGNED)) {
                memset(subset_effect, UNASSIGNED, ASIZE);
                start = start->next;
                i += 1;
            }
        }
        if (memcmp(required, subset_effect, ASIZE)==0) {
            printf("%5d: ", i);
            while (start!=curr) {
                printf("%c", start->action->name);
                start = start->next;
                i+=1;
            }
            printf("\n");
            memset(subset_effect, UNASSIGNED, ASIZE);
        } else {
            apply_effect(curr->action, subset_effect);
            for (j = 0; j < ASIZE; j++) {
                // again, action should't alter variables not set by the routine
                if (required[j] == UNASSIGNED && subset_effect[j] != UNASSIGNED) {
                    while(start!=curr) {
                        start = start->next; i+=1;
                    }
                }
            }
            curr = curr->next;
        }
    }
    // check for the last action in trace
    if (memcmp(required, subset_effect, ASIZE)==0) {
        printf("%5d: ", i);
        while (start!=curr) {
            printf("%c", start->action->name);
            start = start->next; i+=1;
        }
        printf("\n");
    }
}

void handle_stage2(state_t required, state_t subset_effect,
                   state_t subset_precons) {
    // if action alters variables that are not set by the routine, make sure
    // they return back to their original state in the cumulative precondition
    int j;
    for (j = 0; j < ASIZE; j++) {
        if (required[j] == UNASSIGNED && subset_effect[j] != UNASSIGNED) {
            if (subset_effect[j] == subset_precons[j]) {
                // undo the change so comparison becomes easier
                subset_effect[j] = UNASSIGNED;
            }
        }
    }
}

void print_state(state_t arr) {
    int j;
    for (j = 0; j<ASIZE; j++) {
        printf("%d", arr[j]);
    }
    printf("\n");
}

void apply_effect(action_t *a, state_t curr) {
    int i;
    for (i = 0; i<ASIZE; i++) {
        if (a->effect[i] != 2) {
            curr[i] = a->effect[i];
        }
    }
}

/* ALGORITHMS ARE FUN!! */