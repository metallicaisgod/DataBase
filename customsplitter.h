#ifndef CUSTOMSPLITTER_H
#define CUSTOMSPLITTER_H

#include <QSplitter>

class customsplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit customsplitter(QWidget *parent = 0);

signals:

public slots:
    void moveSplitter(int pos, int index)
    {
        QSplitter::moveSplitter(pos, index);
    }

};

#endif // CUSTOMSPLITTER_H
