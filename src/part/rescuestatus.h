#ifndef RESCUESTATUS_H
#define RESCUESTATUS_H

class RescueMap;
class QString;
#include <QMap>

enum class Operation {
    copying,    /* '?': "Copying non-tried blocks" */
    trimming,   /* '*': "Trimming non-trimmed blocks" */
    scraping,  /* '/': "Scraping non-scraped blocks" */
    retrying,   /* '-': "Retrying bad sectors" */
    filling,    /* 'F': "Filling specified blocks" */
    generating, /* 'G': "Generating approximate mapfile" */
    finished,   /* '+': "Finished" */
    unknown,    /*      "Unknown operation" */
};

static QMap<Operation, QString> OperationCharacter {
    {Operation::copying, "?"},
    {Operation::trimming, "*"},
    {Operation::scraping, "/"},
    {Operation::retrying, "-"},
    {Operation::filling, "F"},
    {Operation::generating, "G"},
    {Operation::finished, "+"},
    // {Operation::unknown, " "},
};

static QMap<Operation, QString> OperationDescription {
    {Operation::copying, "Copying non-tried blocks"},
    {Operation::trimming, "Trimming non-trimmed blocks"},
    {Operation::scraping, "Scraping non-scraped blocks"},
    {Operation::retrying, "Retrying bad sectors"},
    {Operation::filling, "Filling specified blocks"},
    {Operation::generating, "Generating approximate mapfile"},
    {Operation::finished, "Finished"},
    {Operation::unknown, "Unknown operation"},
};

class RescueStatus
{
public:
    RescueStatus();
    long long currentPosition() const { return m_current_position; }
    bool setCurrentPosition(long long position);
    Operation currentOperation() const { return m_current_operation; }
    bool setCurrentOperation(QString operation);
    int currentPass() const { return m_current_pass; }
    bool setCurrentPass(int pass);
    bool setProgress(const RescueMap rescue_map);

private:
    long long m_current_position;
    Operation m_current_operation;
    int m_current_pass;
    QMap<Operation, long long> m_progress;

};

#endif // RESCUESTATUS_H
