/********************************************************************************
** Form generated from reading UI file 'exif-scout_layout_20100609.ui'
**
** Created: Wed Jun 16 15:35:28 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXIF_2D_SCOUT_H
#define UI_EXIF_2D_SCOUT_H

#include "ui_exif-scout_layout.h"
#include <qdir.h>
#include <qpixmapcache.h>
#include "clickQlabel.h"


class ExifScout: public QMainWindow, public Ui::MainWindow{
    Q_OBJECT

     public:
            ExifScout(QMainWindow *parent = 0);
            ~ExifScout();            
     private slots:
            void manageSearch();
            void about();
            void showPic1Full();
            void showPic2Full();
            void showPic3Full();
            void showPic4Full();
            void openDirDialog();
            void changeShownImages(int toNumber);
            void showImages(int startAtPictureNumber);
            void cleanUpForNewSearch();
            void reloadComboBoxOfValues1(int cBoxTag);
            void reloadComboBoxOfValues2(int cBoxTag);
            void reloadComboBoxOfValues3(int cBoxTag);
            void activateSMask2(bool bl);
            void activateSMask3(bool bl);
     private:
            std::vector<QStringList> valuesOfCBoxValue;
            QPixmapCache previewPixmaps;
            QStringList foundFiles;
            QString translateCBoxTagToExifTag(int cBoxTag);
            QPixmap pixmapFromImage(const QString path, const QLabel &labelObject);
            QAction *zoomInAct;
            QAction *zoomOutAct;
            QString getExifData(QString filename, QString exiftag);
            void checkExifForAll(QStringList &files, QString tagname, QString value);
            void showPicInMainArea(QString pictureName);
            void generatePixmaps(const QLabel &labelObject);
	    void addFile(QString fileToAdd);           
            void searchDirectoryRecursive(QString directoryPath);
            void initialisations();
            int showExifData(std::string a);
};


#endif // UI_EXIF_2D_SCOUT_LAYOUT_20100609_H
