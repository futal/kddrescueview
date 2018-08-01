#ifndef RESCUEMAP_H
#define RESCUEMAP_H

#include <QStandardItemModel>

enum class Status {
    nontried,   /* '?': "Non-tried block" */
    nontrimmed, /* '*': "Failed block non-trimmed" */
    nonscraped,   /* '/': "Failed block non-scraped" */
    badsector,  /* '-': "Failed block bad-sector(s)" */
    rescued,    /* '+': "Finished block (rescued)" */
    unknown,    /*      "Unknown status */
};
    
static QMap<Status, QString> StatusCharacter {
    {Status::nontried, "?"},
    {Status::nontrimmed, "*"},
    {Status::nonscraped, "/"},
    {Status::badsector, "-"},
    {Status::rescued, "+"},
    //{Status::unknown, " "},
};

static QMap<Status, QString> StatusDescription {
    {Status::nontried, "Non-tried block"},
    {Status::nontrimmed, "Failed block non-trimmed"},
    {Status::nonscraped, "Failed block non-scraped"},
    {Status::badsector, "Failed block bad-sector(s)"},
    {Status::rescued, "Finished block (rescued)"},
    {Status::unknown, "Unknown status"},
};

static QMap<Status, QColor> StatusColor {
    {Status::nontried, QColor(QRgb(0x404040))},
    {Status::nontrimmed, QColor(QRgb(0x00e0ff))},
    {Status::nonscraped, QColor(QRgb(0xff2020))},
    {Status::badsector, QColor(QRgb(0x0000ff))},
    {Status::rescued, QColor(QRgb(0x20e020))},
    {Status::unknown, QColor(Qt::gray)},
};

const QColor active_color = QColor(QRgb(0xffff00));

static QMap<Status, int> StatusWeight {
    {Status::nontried, 1},
    {Status::rescued, 2},
    {Status::nontrimmed, 4},
    {Status::nonscraped, 10},
    {Status::badsector, 40},
};

const QBrush active_brush = QBrush(QRgb(0xffff00));

static QMap<Status, QBrush> StatusBrush {
    {Status::nontried, QBrush(QRgb(0x404040))},
    {Status::nontrimmed, QBrush(QRgb(0x00e0ff))},
    {Status::nonscraped, QBrush(QRgb(0xff2020))},
    {Status::badsector, QBrush(QRgb(0x0000ff))},
    {Status::rescued, QBrush(QRgb(0x20e020))},
    {Status::unknown, QBrush(Qt::gray)},
};

class BlockData
{
public:
    BlockData(long long position, long long size, QString Status);
    long long position() const { return m_position; }
    long long size() const { return m_size; }
    Status status() const { return m_status; }
private:
    long long m_position;
    long long m_size;
    Status m_status;
};


class RescueMap : public QStandardItemModel
{
public:
    RescueMap(QObject *parent = nullptr);
    void addRescueBlock(long long position, long long size, QString operation);
    long long lastPosition() const;
    long long lastSize() const;
    void print() const;
private:
    QList<BlockData> m_rescue_map;
    
};

#endif // RESCUEMAP_H
