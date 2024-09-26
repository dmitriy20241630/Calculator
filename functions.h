#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

enum EAction
{
    Action_None = 0,
    Action_Div,
    Action_Mul,
    Action_Add,
    Action_Sub
};

int performAction(enum EAction action, const char *first, const char *second, char **result);

#ifdef __cplusplus
}
#endif

#endif // FUNCTIONS_H
