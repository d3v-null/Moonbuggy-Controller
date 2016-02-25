#ifndef BuggyController_h
#define BuggyController_h

typedef enum {S_SAFE, S_TERMINATING, S_TERMINATED}  safetyStatusType;
typedef enum {TH_ZERO, TH_NORMAL, TH_BOOST}         throttleStatusType;
typedef struct throttleStatusNode {
    double threshold; 
    throttleStatusType statusVal;
} throttleStatusNode;

#endif