#include "rescuestatus.h"

#include <QDebug>

RescueStatus::RescueStatus()
{
    m_current_position = 0;
    m_current_operation = Operation::unknown;
    m_current_pass = -1;
    m_progress = {
        {Operation::copying, 0},
        {Operation::trimming, 0},
        {Operation::scraping, 0},
        {Operation::retrying, 0},
        {Operation::filling, 0},
        {Operation::generating, 0},
        {Operation::finished, 0},
        {Operation::unknown, 0},
    };
}

bool RescueStatus::setCurrentPosition(long long position)
{
    if( position < 0 ) {
        return false;
    }
    m_current_position = position;
    return true;
}

bool RescueStatus::setCurrentOperation(QString operation)
{
    if( operation == OperationCharacter[Operation::copying] ) {
        m_current_operation = Operation::copying;
        return true;
    } else if( operation == OperationCharacter[Operation::trimming] ) {
        m_current_operation = Operation::trimming;
        return true;
    } else if( operation == OperationCharacter[Operation::scraping] ) {
        m_current_operation = Operation::scraping;
        return true;
    } else if( operation == OperationCharacter[Operation::retrying] ) {
        m_current_operation = Operation::retrying;
        return true;
    } else if( operation == OperationCharacter[Operation::filling] ) {
        m_current_operation = Operation::filling;
        return true;
    } else if( operation == OperationCharacter[Operation::generating] ) {
        m_current_operation = Operation::generating;
        return true;
    } else if( operation == OperationCharacter[Operation::finished] ) {
        m_current_operation = Operation::finished;
        return true;
    } else {
        m_current_operation = Operation::unknown;
        return false;
    }
}

bool RescueStatus::setCurrentPass(int pass)
{
    if( pass <= 0 ) {
        qDebug() << "Error: pass number must be greater than 1";
        return false;
    }
    m_current_pass = pass;
    return true;
}

/*
void RescueStatus::setProgress(const RescueMap rescue_map)
{
    
}
*/
