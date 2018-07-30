#ifndef RESCUESTATUS_H
#define RESCUESTATUS_H

class RescueMap;
class QString;
#include <QMap>

enum class Operation {
    copying,    /* '?': "Copying non-tried" */
    trimming,   /* '*': "Trimming failed blocks" */
    splitting,  /* '/': "Splitting failed blocks" */
    retrying,   /* '-': "Retrying bad sectors" */
    filling,    /* 'F': "Filling blocks" */
    generating, /* 'G': "Generating logfile" */
    finished,   /* '+': "Finished" */
    unknown,    /* "Unknown operation" */
};

static QMap<Operation, char> Character {
    {Operation::copying, '?'},
    {Operation::trimming, '*'},
    {Operation::splitting, '/'},
    {Operation::retrying, '-'},
    {Operation::filling, 'F'},
    {Operation::generating, 'G'},
    {Operation::finished, '+'},
    {Operation::unknown, ' '},
};

static QMap<Operation, QString> Description {
    {Operation::copying, "Copying non-tried"},
    {Operation::trimming, "Trimming failed blocks"},
    {Operation::splitting, "Splitting failed blocks"},
    {Operation::retrying, "Retrying bad sectors"},
    {Operation::filling, "Filling blocks"},
    {Operation::generating, "Generating map file"},
    {Operation::finished, "Finished"},
    {Operation::unknown, "Unknown operation"},
};

class RescueStatus
{
public:
    RescueStatus();

//    void setCurrentRescueStatus(int position, char status);
//    void setRescueProgress(const RescueMap rescue_map);

private:
    QMap<QString, QString> operation;
    
    int current_position;
    char current_status;
    Operation current_operation;
    
    QMap<Operation, long long int> current_progress;
    
};

#endif // RESCUESTATUS_H
