#include "widget.h"

#include <QApplication>
#include <QMessageBox>

Widget::Widget(QWidget *parent) : QWidget(parent) {
  bool running = true;

  do {
    QFile inFileSource(QFileDialog::getOpenFileName(
        this, tr("Open File"),
        "/home/andre/Dropbox/Internet/fiverr und co/Jobs/wondershare/source/",
        tr("*.*")));
    if (!inFileSource.open(QFile::ReadOnly | QFile::Text)) {
      qDebug() << "ERROR: Unable to open input file: "
               << "file.txt";
      exit(1);
    }
    QTextStream inStreamSource(&inFileSource);
    QString fileContentsSource = inStreamSource.readAll();

    QFile inFileTarget(QFileDialog::getOpenFileName(
        this, tr("Open File"),
        "/home/andre/Dropbox/Internet/fiverr und co/Jobs/wondershare/target/",
        tr("*.*")));
    if (!inFileTarget.open(QFile::ReadOnly | QFile::Text)) {
      qDebug() << "ERROR: Unable to open input file: "
               << "file.txt";
      exit(1);
    }
    QTextStream inStreamTarget(&inFileTarget);
    QString fileContentsTarget = inStreamTarget.readAll();

    QRegularExpression reSource("<.*?>");
    QRegularExpression reWhitespaceSource("\\s",
                                          QRegularExpression::MultilineOption);
    QRegularExpression reWhitespaceTarget("\\s",
                                          QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator i =
        reSource.globalMatch(fileContentsSource);

    int startPosTarget = 0;

    while (i.hasNext()) {
      QRegularExpressionMatch matchSource = i.next();
      QString match = matchSource.captured(0);
      // c qDebug() << "Source: " << matchSource.capturedStart() << " - " <<
      // match;

      // Find in target

      if (matchSource.captured(0).contains("alt=")) {
        qDebug() << "Alt found, ignoring";
        continue;
      } else {
        startPosTarget = fileContentsTarget.indexOf(match, startPosTarget);
      }

      if (startPosTarget != -1) {
        // We found the same tag

        // Check for whitespace before

        // Collect as many whitespaces as possible before

        QString beforeSourceCollected;
        int j = 1;
        bool running = true;

        do {
          QString beforeSource =
              fileContentsSource.mid(matchSource.capturedStart() - j, 1);

          QRegularExpressionMatch whitespaceMatchSource =
              reWhitespaceSource.match(beforeSource);

          if (whitespaceMatchSource.hasMatch()) {
            j++;
            beforeSourceCollected =
                whitespaceMatchSource.captured(0) + beforeSourceCollected;
          } else {
            running = false;
          }
        } while (running && startPosTarget - j > 0);

        // qDebug() << beforeSourceCollected;

        // Collect whitespace before target tag

        QString beforeTargetCollected;
        j = 1;
        running = true;

        do {
          QString beforeTarget = fileContentsTarget.mid(startPosTarget - j, 1);

          QRegularExpressionMatch whitespaceMatchTarget =
              reWhitespaceTarget.match(beforeTarget);

          if (whitespaceMatchTarget.hasMatch()) {
            j++;
            beforeTargetCollected =
                whitespaceMatchTarget.captured(0) + beforeTargetCollected;
          } else {
            running = false;
          }
        } while (running && startPosTarget - j > 0);

        // Check if target matches source
        if (beforeSourceCollected.compare(beforeTargetCollected) == 0) {
          // Whitespaces match, nothing to do
        } else {
          // qDebug() << "Replace in Target: " << startPosTarget;

          // Copy whitespace from source to target
          if (beforeSourceCollected.length() == 0) {
            fileContentsTarget.remove(startPosTarget -
                                          beforeTargetCollected.length(),
                                      beforeTargetCollected.length());
          } else {
            fileContentsTarget.replace(
                startPosTarget - beforeTargetCollected.length(),
                beforeTargetCollected.length(), beforeSourceCollected);
          }
        }
      } else {
        qDebug() << "Tag not found in Target!";
      }
    }

    startPosTarget = 0;

    i = reSource.globalMatch(fileContentsSource);
    while (i.hasNext()) {
      QRegularExpressionMatch matchSource = i.next();
      QString match = matchSource.captured(0);
      // qDebug() << match;

      // Find in target
      startPosTarget = fileContentsTarget.indexOf(match, startPosTarget);

      if (startPosTarget != -1) {
        // --- After

        // Check for whitespace after

        // Collect as many whitespaces as possible after

        QString afterSourceCollected;
        int j = 0;
        bool running = true;

        do {
          QString afterSource =
              fileContentsSource.mid(matchSource.capturedEnd() + j, 1);

          QRegularExpressionMatch whitespaceMatchSource =
              reWhitespaceSource.match(afterSource);

          if (whitespaceMatchSource.hasMatch()) {
            j++;
            afterSourceCollected += whitespaceMatchSource.captured(0);
          } else {
            running = false;
          }
        } while (running && startPosTarget + j < fileContentsSource.length());

        qDebug() << afterSourceCollected;

        // Collect whitespace after target tag

        QString afterTargetCollected;
        j = 0;
        running = true;
        int endPosTarget = startPosTarget + match.length();

        do {
          QString afterTarget = fileContentsTarget.mid(endPosTarget + j, 1);

          QRegularExpressionMatch whitespaceMatchTarget =
              reWhitespaceTarget.match(afterTarget);

          if (whitespaceMatchTarget.hasMatch()) {
            j++;
            afterTargetCollected += whitespaceMatchTarget.captured(0);
          } else {
            running = false;
          }
        } while (running && startPosTarget + j < fileContentsTarget.length());

        // Check if target matches source
        if (afterSourceCollected.compare(afterTargetCollected) == 0) {
          // Whitespaces match, nothing to do
        } else {
          // Copy whitespace from source to target
          if (afterSourceCollected.length() == 0) {
            fileContentsTarget.remove(endPosTarget,
                                      afterTargetCollected.length());
          } else {
            if (afterTargetCollected.length() == 0) {
              fileContentsTarget.insert(endPosTarget, afterSourceCollected);
            } else {
              fileContentsTarget.replace(endPosTarget,
                                         afterTargetCollected.length(),
                                         afterSourceCollected);
            }
          }
        }
      }
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

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Another file?", "Another file?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
      running = false;
    }

  } while (running);

  exit(0);
}

Widget::~Widget() {}

void Widget::checkBefore() {}
