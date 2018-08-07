#include "widget.h"

#include <QApplication>
#include <QMessageBox>

// struct Change {
//  int tagID;
//  int
//};

Widget::Widget(QWidget *parent) : QWidget(parent) {

  QStringList sourceFiles = QFileDialog::getOpenFileNames(
      this, tr("Open File"),
      "/home/andre/Dropbox/Internet/fiverr und co/Jobs/wondershare/source/",
      tr("*.*"));

  QStringList targetFiles = QFileDialog::getOpenFileNames(
      this, tr("Open File"),
      "/home/andre/Dropbox/Internet/fiverr und co/Jobs/wondershare/target/",
      tr("*.*"));

  // Check if we got the same source and target files
  if (sourceFiles.length() == targetFiles.length()) {
    QStringListIterator itSource(sourceFiles);
    QStringListIterator itTarget(targetFiles);
    while (itSource.hasNext() && itTarget.hasNext()) {
      QString sourceFile = itSource.next();
      QString targetFile = itTarget.next();

      if (sourceFile.right(sourceFile.length() - sourceFile.lastIndexOf("/"))
              .compare(targetFile.right(targetFile.length() -
                                        targetFile.lastIndexOf("/"))) == 0) {

        QFile inFileSource(sourceFile);
        if (!inFileSource.open(QFile::ReadOnly | QFile::Text)) {
          qDebug() << "ERROR: Unable to open input file: "
                   << "file.txt";
          exit(1);
        }
        QTextStream inStreamSource(&inFileSource);
        QString fileContentsSource = inStreamSource.readAll();

        QFile inFileTarget(targetFile);
        if (!inFileTarget.open(QFile::ReadOnly | QFile::Text)) {
          qDebug() << "ERROR: Unable to open input file: "
                   << "file.txt";
          exit(1);
        }
        QTextStream inStreamTarget(&inFileTarget);
        QString fileContentsTarget = inStreamTarget.readAll();

        QRegularExpression reTags("<.*?>");
        QRegularExpression reWhitespace("\\s",
                                        QRegularExpression::MultilineOption);
        QRegularExpressionMatchIterator i =
            reTags.globalMatch(fileContentsSource);

        QRegularExpressionMatchIterator j =
            reTags.globalMatch(fileContentsTarget);

        int tagsSource = 0;
        int tagsTarget = 0;

        tagsSource = countTags(i);
        tagsTarget = countTags(j);

        if (tagsSource != tagsTarget) {
          qDebug() << "Tag numbers do not match!";
          break;
        }

        // Todo: Find way to reset iterators

        i = reTags.globalMatch(fileContentsSource);

        int tagSourceID = 0;

        // Go through tags

        while (i.hasNext()) {
          QRegularExpressionMatch matchSource = i.next();
          QString matchSourceText = matchSource.captured();

          // Get whitespace before and after source tag

          QString beforeSourceCollected = whitespacesBefore(
              fileContentsSource.left(matchSource.capturedStart()));

          QString afterSourceCollected = whitespacesAfter(
              fileContentsSource.mid(matchSource.capturedEnd()));

          // Find same tag ID in target

          j = reTags.globalMatch(fileContentsTarget);

          // skip to same target tag one we want

          QRegularExpressionMatch matchTarget = j.next();

          for (int x = 0; x < tagSourceID; x++) {
            matchTarget = j.next();
          }

          QString matchTargetText = matchTarget.captured();

          QString beforeTargetCollected = whitespacesBefore(
              fileContentsTarget.left(matchTarget.capturedStart()));

          if (beforeSourceCollected.compare(beforeTargetCollected) != 0) {

            // Copy whitespace from source to target

            if (beforeSourceCollected.length() == 0) {
              // There’s no whitespace in the source file, so delete everything
              // in the target file
              fileContentsTarget.remove(matchTarget.capturedStart() -
                                            beforeTargetCollected.length(),
                                        beforeTargetCollected.length());
            } else {
              // ...otherwise copy over the whitespace
              fileContentsTarget.replace(
                  matchTarget.capturedStart() - beforeTargetCollected.length(),
                  beforeTargetCollected.length(), beforeSourceCollected);
            }
          }

          // TODO: Match again necessary for whitespace after tag, need a way to
          // do this more elegantly

          j = reTags.globalMatch(fileContentsTarget);

          // skip to same target tag one we want

          matchTarget = j.next();

          for (int x = 0; x < tagSourceID; x++) {
            matchTarget = j.next();
          }

          QString afterTargetCollected = whitespacesAfter(
              fileContentsTarget.mid(matchTarget.capturedEnd()));

          if (afterSourceCollected.compare(afterTargetCollected) != 0) {
            // Copy whitespace from source to target

            if (afterSourceCollected.length() == 0) {
              // There’s no whitespace in the source file, so
              // delete everything in the target file

              fileContentsTarget.remove(matchTarget.capturedEnd(),
                                        afterTargetCollected.length());
            } else {
              // ...otherwise copy over the whitespace

              fileContentsTarget.replace(matchTarget.capturedEnd(),
                                         afterTargetCollected.length(),
                                         afterSourceCollected);
            }
          }

          tagSourceID++;
        }

        inFileSource.close();
        inFileTarget.close();

        // Write target
        QFile outFile(inFileTarget.fileName());
        if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
          qCritical() << "ERROR: Unable to open output file: "
                      << "file.txt";
          exit(1);
        }

        QTextStream outStream(&outFile);
        outStream << fileContentsTarget;
        outFile.close();
      }
    }
  }

  exit(0);
}

Widget::~Widget() {}

QString Widget::whitespacesBefore(QString string) {
  // Get string up to position of current tag and find all whitespaces
  // before it

  QString whitespaces;

  QRegularExpression reWhitespaces("\\s+$");
  QRegularExpressionMatch reWhitespacesMatch = reWhitespaces.match(string);

  if (reWhitespacesMatch.hasMatch()) {
    whitespaces = reWhitespacesMatch.captured(0);
  }
  return whitespaces;
}

QString Widget::whitespacesAfter(QString string) {
  // Get from end of current tag and find all whitespaces
  // after it

  QString whitespaces;

  QRegularExpression reWhitespaces("^\\s+");
  QRegularExpressionMatch reWhitespacesMatch = reWhitespaces.match(string);

  if (reWhitespacesMatch.hasMatch()) {
    whitespaces = reWhitespacesMatch.captured(0);
  }
  return whitespaces;
}

int Widget::countTags(QRegularExpressionMatchIterator i) {
  int number = 0;
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    number++;
  }
  return number;
}
