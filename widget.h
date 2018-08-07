#ifndef WIDGET_H
#define WIDGET_H

#include <QFile>
#include <QFileDialog>
#include <QRegularExpression>
#include <QTextStream>
#include <QWidget>
#include <QtDebug>

class Widget : public QWidget {
  Q_OBJECT

public:
  Widget(QWidget *parent = nullptr);
  ~Widget();

private:
  QString whitespacesBefore(QString string);
  QString whitespacesAfter(QString string);
  int countTags(QRegularExpressionMatchIterator i);
};

#endif // WIDGET_H
