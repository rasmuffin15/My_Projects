#include "dfHeader.h"


filenode *fn_new_filenode(char *name)
{
    filenode *fn = malloc(sizeof(struct fn));
    fn->name = strdup(name);
    for (int i = 0; i < 4; i ++)
        fn->fragments[i] = 0;
    fn->next = NULL;
    return (fn);
}

void fn_setframent(filenode *fn, int fragment)
{
    fn->fragments[fragment -1] = 1;
}

filenode *fn_findbyname(filenode **root, char *name) {

    filenode *fn_tmp;
    fn_tmp = *root;
    while (fn_tmp != NULL) {
        if (strcmp(fn_tmp->name, name) == 0)
            return (fn_tmp);
        fn_tmp = fn_tmp->next;
    }
    return (NULL);
}

int fn_getfragments(filenode *fn) {
    int r = 0;
    for (int i=0; i < 4; i++)
        r += fn->fragments[i];
    return r;
};

void fn_push(filenode *fn, filenode **root) {
    filenode *fn_tmp;
    fn_tmp = *root;
    if (*root == NULL) {
        *root = fn;
        return;
    }

    while (fn_tmp->next != NULL)
        fn_tmp = fn_tmp -> next;
    fn_tmp-> next = fn;
}

void fn_printall(filenode **root)
{
    wchar_t checkmark = 0x2705; // Green check mark
    wchar_t nogood = 0x274C; // red cross mark.
    filenode *fn_tmp;
    fn_tmp = *root;
    while (fn_tmp != NULL) {
        printf("%s ", fn_tmp->name);
        int nf;
        if ((nf = fn_getfragments(fn_tmp)) == 4) {
            printf(" (OK)\n");
        } else {
            printf(" (MISSING FRAGMENTS)\n");
        }
        fn_tmp = fn_tmp->next;
    }
}

void fn_cleanup(filenode **root) {
    filenode *fn_tmp;
    fn_tmp = *root;
    while (fn_tmp != NULL) {
        filenode *t = fn_tmp;
        fn_tmp = fn_tmp->next;
        free(t);
    }
}