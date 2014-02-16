/* Exif-Scout Window Class
    v1.2
        2010-07-14
            Tim Luedtke */

#include <QtCore/QVariant>
#include <QtGui>
#include <QPixmap>
#include <QHttp>
#include <QComboBox>
#include <iomanip>
#include <iostream>
#include <dirent.h>
#include <cassert>
#include <string>
#include <qdir.h>
#include <exiv2/src/image.hpp>
#include <exiv2/src/exif.hpp>

#include "exif-scout_window.h"

extern const char* version;
using namespace std;

ExifScout::ExifScout(QMainWindow *parent) : QMainWindow(parent){
    setupUi(this);
    setWindowTitle(tr(version));
    previewPixmaps.setCacheLimit(131072);

    Ui_MainWindow::localpath->setText("./");
    Ui_MainWindow::area_searchat->disconnect();
    Ui_MainWindow::horizontalScrollBar->setMinimum(1);
    Ui_MainWindow::horizontalScrollBar->setMaximum(1);
    Ui_MainWindow::horizontalScrollBar->setPageStep(1);
    Ui_MainWindow::label_showingPicture->setText("Start a search request!");
    Ui_MainWindow::statusBar->showMessage("Program started", 5000);

    connect(actionExit,SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(actionAbout,SIGNAL(triggered()), this, SLOT(about()));
    connect(searchButton,SIGNAL(clicked()), this, SLOT(manageSearch()));
    connect(button_getDirectory,SIGNAL(clicked()),this,SLOT(openDirDialog()));
    connect(prevImageLabel1,SIGNAL(clicked()), this, SLOT(showPic1Full()));
    connect(prevImageLabel2,SIGNAL(clicked()), this, SLOT(showPic2Full()));
    connect(prevImageLabel3,SIGNAL(clicked()), this, SLOT(showPic3Full()));
    connect(prevImageLabel4,SIGNAL(clicked()), this, SLOT(showPic4Full()));
    connect(horizontalScrollBar,SIGNAL(valueChanged(int)),this,SLOT(changeShownImages(int)));
    connect(pushButton_newSearch, SIGNAL(clicked()),this,SLOT(cleanUpForNewSearch()));
    connect(exiftag1, SIGNAL(currentIndexChanged(int)),this,SLOT(reloadComboBoxOfValues1(int)));
    connect(exiftag2, SIGNAL(currentIndexChanged(int)),this,SLOT(reloadComboBoxOfValues2(int)));
    connect(exiftag3, SIGNAL(currentIndexChanged(int)),this,SLOT(reloadComboBoxOfValues3(int)));
    connect(checkBox_more2, SIGNAL(clicked(bool)),this,SLOT(activateSMask2(bool)));
    connect(checkBox_more3, SIGNAL(clicked(bool)),this,SLOT(activateSMask3(bool)));

    // fill the QComboBoxValue-Array with examplevalues
    QStringList sl;
    sl << "Canon" << "Fujifilm" << "Kodak" << "Leica" << "Nikon" << "Olympus" << "Panasonic" << "Pentax" << "Sony";
    valuesOfCBoxValue.push_back(sl);
    sl.clear();
    sl << "YYYY:MM:DD HH:MM:SS" << "2010:11:08 *" << "2010*";
    valuesOfCBoxValue.push_back(sl);
    sl.clear();
    sl << "1/500" << "1/??" << "1/[12345]??";
    valuesOfCBoxValue.push_back(sl);
    sl.clear();
    sl << "100" << "200" << "400" << "800" << "1600" << "[1248]00";
    valuesOfCBoxValue.push_back(sl);
    sl.clear();
    // 1  1.4  2  2.8  4  5.6  8  11  16  22
    sl << "1/1" << "14/10" << "2/1" << "28/10" << "4/1" << "56/10" << "8/1" << "11/1" << "16/1" << "22/1";
    valuesOfCBoxValue.push_back(sl);

    initialisations();
}


ExifScout::~ExifScout(){
}

// to fill the ComboBoxes right
void ExifScout::initialisations(){
    Ui_MainWindow::exiftag2->setCurrentIndex(1);
    Ui_MainWindow::exiftag3->setCurrentIndex(2);
}

// makes search with more than one searchinput possible
void ExifScout::activateSMask2(bool bl){
    Ui_MainWindow::exiftag2->setEnabled(bl);
    Ui_MainWindow::value2->setEnabled(bl);
    Ui_MainWindow::checkBox_more3->setEnabled(bl);
    if(!bl) {
        Ui_MainWindow::exiftag3->setEnabled(false);
        Ui_MainWindow::value3->setEnabled(false);
        Ui_MainWindow::checkBox_more3->setChecked(false);
    }
}
void ExifScout::activateSMask3(bool bl){
    Ui_MainWindow::exiftag3->setEnabled(bl);
    Ui_MainWindow::value3->setEnabled(bl);
}


// popup for selecting a path
void ExifScout::openDirDialog(){
    QString temp = (QFileDialog::getExistingDirectory(this, tr("Choose Directory"), QDir::currentPath()));
        if(temp != "") { localpath->setText(temp); }
}


// changes the previewed images
void ExifScout::changeShownImages(int toNumber){
    showImages(toNumber-1);
}


// show Image in main_label, when previewLabel it clicked
void ExifScout::showPic1Full(){
    showPicInMainArea(Ui_MainWindow::prevImageLabel1->whatsThis());
}
void ExifScout::showPic2Full(){
    showPicInMainArea(Ui_MainWindow::prevImageLabel2->whatsThis());
}
void ExifScout::showPic3Full(){
    showPicInMainArea(Ui_MainWindow::prevImageLabel3->whatsThis());
}
void ExifScout::showPic4Full(){
    showPicInMainArea(Ui_MainWindow::prevImageLabel4->whatsThis());
}


// uses the exiv2 class to readout the exifdata from images
int ExifScout::showExifData(std::string a){
    try {
        Ui_MainWindow::textEdit->clear();
        std::cerr << "ImageFactory::open ";
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(a);
        std::cerr << "DONE\n";
        assert(image.get() != 0);

        std::cerr << "image->readMetadata() ";
        image->readMetadata();
        std::cerr << "DONE\n";

        std::cerr << "Exiv2::ExifData& exifData = image->exifData() ";
        Exiv2::ExifData exifData = image->exifData();
        std::cerr << "DONE\n";

	cout << "Number of found exiv2 data: " << exifData.count() << endl;
        if (exifData.empty()) {
            Ui_MainWindow::textEdit->setPlainText("No Exif data found in file");
            return 1;
        }

        Exiv2::ExifData::const_iterator end = exifData.end();
        std::cerr << "Generate the exif-output ";
        QString output;
        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
            std::string tagname = i->tagName();

            if(tagname != std::string("MakerNote") and
               tagname.substr(0,2) != "0x"){
                // Print out the exif-data in the QtextEdit Field
                std::ostringstream oss;
                oss << i->tagName() << ":\n    " << i->value() << "\n";
                output = output+(QString)oss.str().c_str();
            }
        }
        std::cerr << "Done\n";
        Ui_MainWindow::textEdit->setPlainText(output);

        return 0;
    }
    //catch (std::exception& e) {
    //catch (Exiv2::AnyError& e) {
    catch (Exiv2::Error& e) {
        std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
        return -1;
    }
}


// tries to find a specified exif tag in a specified file
QString ExifScout::getExifData(QString fname, QString etag){
    std::string filename = fname.toAscii().data();
    std::string exiftag = etag.toAscii().data();

    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
    assert(image.get() != 0);
    image->readMetadata();
    Exiv2::ExifData exifData = image->exifData();
    if (!exifData.empty()) {
        Exiv2::ExifData::const_iterator end = exifData.end();
        QString output;
        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
            if(i->tagName()==exiftag){
                std::ostringstream oss;
                oss << i->value();
                return (QString)oss.str().c_str();
            }
        }
    }
    return NULL;
}


// searches a given path recursive and return found pathes to picture files (JPEG)
void ExifScout::searchDirectoryRecursive(QString directoryPath){
    std::cerr << "Folder: " << (std::string)directoryPath.toAscii() << "\n";

    // search specifications
    QDir currentDir = QDir(directoryPath);
    currentDir.setSorting( QDir::Name);
    currentDir.setFilter( QDir::Files | QDir::Dirs );
    QStringList files = currentDir.entryList(QStringList("*.jpg"));
    QStringList dirs = currentDir.entryList(QDir::Dirs, QDir::Name);

    for(int i=0;i<files.size();i++) {
        files[i] = directoryPath+"/"+files[i];
    }
    foundFiles.append(files);

    for(int i=0;i<dirs.size();i++) {
        if (dirs[i] != "." and dirs[i] != "..")
            searchDirectoryRecursive(directoryPath+"/"+dirs[i]);
    }

    // write found files to console
    std::cerr << foundFiles.size() << " Files found. \n";
       /* for(int i = 0; i < foundFiles.size(); i++){
            std::cerr << foundFiles.at(i).toStdString() << "\n";
        }*/
}


// generates the bigger pixmap for the main area and prints its exif data
void ExifScout::showPicInMainArea(QString pictureName){
    if(pictureName!="")
    {
        Ui_MainWindow::prevImageLabel_main->setPixmap(ExifScout::pixmapFromImage(pictureName, *Ui_MainWindow::prevImageLabel_main));
        Ui_MainWindow::prevImageLabel_main->setWhatsThis(pictureName); //save the path for later reconition
        Ui_MainWindow::prevImageLabel_main->show();

        showExifData((std::string)pictureName.toAscii());
    }
}


// is needed to search recursive
void ExifScout::addFile(QString fileToAdd){
    foundFiles.append(fileToAdd);
}


// makes the GUI clean and ready for a new search
void ExifScout::cleanUpForNewSearch(){
    Ui_MainWindow::horizontalScrollBar->setMaximum(1);
    Ui_MainWindow::textEdit->clear();
    Ui_MainWindow::label_showingPicture->setText("Start a search request!");
    Ui_MainWindow::keywordsLabel->setText("Keywords: ");
    Ui_MainWindow::prevImageLabel1->clear();
    Ui_MainWindow::prevImageLabel1->setWhatsThis("");
    Ui_MainWindow::prevImageLabel2->clear();
    Ui_MainWindow::prevImageLabel2->setWhatsThis((QString)"");
    Ui_MainWindow::prevImageLabel3->clear();
    Ui_MainWindow::prevImageLabel3->setWhatsThis((QString)"");
    Ui_MainWindow::prevImageLabel4->clear();
    Ui_MainWindow::prevImageLabel4->setWhatsThis((QString)"");
    Ui_MainWindow::prevImageLabel_main->clear();
    foundFiles.clear();
    previewPixmaps.clear();
}


// checks all found pictures of matching with entered exif values
void ExifScout::checkExifForAll(QStringList &files, QString tagname, QString value){
    QStringList matchedFiles;
    Ui_MainWindow::keywordsLabel->setText(Ui_MainWindow::keywordsLabel->text()+value+"  ");

    for (QStringList::const_iterator it = files.constBegin(); it != files.constEnd(); ++it){
        QString exifValue = getExifData((*it),tagname);
        std::cerr << "Tag: "<< tagname.toAscii().data() << " Value: " << value.toAscii().data();

        //QStringList ltemp;
        //ltemp << exifValue;
        QRegExp rx = QRegExp(value, Qt::CaseSensitive, QRegExp::WildcardUnix);
        //retemp.setWildcard(true);
        //QStringList lt2 = ltemp.filter(retemp);
        //if (!lt2.isEmpty()) {
        if (rx.exactMatch(exifValue)){
            std::cerr << "  true\n";
            matchedFiles.append(*it);
        }
        else {
            std::cerr << "  false\n";
        }
    }
    files = matchedFiles;
}


// is needed to set the right exif-tags
QString ExifScout::translateCBoxTagToExifTag(int cBoxTag){
    QString ca[] = {"Make", "DateTime", "ExposureTime", "ISOSpeedRatings", "FNumber"};
    return ca[cBoxTag];
}


//update the values in the CBoxValue, when changes made in CBoxExifTag
void ExifScout::reloadComboBoxOfValues1(int cBoxTag){
    std::cerr << "reload to: " << cBoxTag << "\n";
    Ui_MainWindow::value1->clear();
    Ui_MainWindow::value1->addItems(valuesOfCBoxValue.at(cBoxTag));
    Ui_MainWindow::value1->setEditText("*");
}
void ExifScout::reloadComboBoxOfValues2(int cBoxTag){
    Ui_MainWindow::value2->clear();
    Ui_MainWindow::value2->addItems(valuesOfCBoxValue.at(cBoxTag));
    Ui_MainWindow::value2->setEditText("*");
}
void ExifScout::reloadComboBoxOfValues3(int cBoxTag){
    Ui_MainWindow::value3->clear();
    Ui_MainWindow::value3->addItems(valuesOfCBoxValue.at(cBoxTag));
    Ui_MainWindow::value3->setEditText("*");
}


// show an Image specified by the Path in the UI
void ExifScout::manageSearch(){
    cleanUpForNewSearch();
    QString path = localpath->text();    
    Ui_MainWindow::label_showingPicture->setText("Searching . . .");
    //QHttp::get("http://rr97.de/Images/PublicDomain/protection_obligatoire__10.gif");  
    std::cerr << "searching Directorys\n";
    searchDirectoryRecursive(path);

    Ui_MainWindow::label_showingPicture->setText("Loading images . . .");

    if(!foundFiles.isEmpty()) {
        checkExifForAll(foundFiles, translateCBoxTagToExifTag(Ui_MainWindow::exiftag1->currentIndex()), Ui_MainWindow::value1->currentText());
        if(Ui_MainWindow::checkBox_more2->isChecked()) {
            checkExifForAll(foundFiles, translateCBoxTagToExifTag(Ui_MainWindow::exiftag2->currentIndex()), Ui_MainWindow::value2->currentText());
        }
        if(Ui_MainWindow::checkBox_more3->isChecked()) {
            checkExifForAll(foundFiles, translateCBoxTagToExifTag(Ui_MainWindow::exiftag3->currentIndex()), Ui_MainWindow::value3->currentText());
        }

        //std::cerr << "finished. Now showing Main picture.\n";
        if(!foundFiles.isEmpty()){
            showPicInMainArea(foundFiles.at(0));
            std::cerr << "finished. ";

            // actualize the scrollbar
            int newSize = foundFiles.size()-3;
            if(newSize<1) newSize = 1;
            Ui_MainWindow::horizontalScrollBar->setMaximum(newSize);

            Ui_MainWindow::label_showingPicture->setText("Generating previews . . . this could take a while");
            std::cerr << "Generate the pixmaps now! \n";
            generatePixmaps(*Ui_MainWindow::prevImageLabel1);
            Ui_MainWindow::label_showingPicture->setText("Show Images");
            std::cerr << "finished. Now show Images.\n";
            showImages(0);        
       }
       else {
            std::cerr << "0 Files found after Exif-Search\n";
       }
    }
    else {
        Ui_MainWindow::label_showingPicture->setText("No Pictures found.");
    }

    Ui_MainWindow::label_showingPicture->setText(QString::number(foundFiles.size())+" Images found");
}


// loads all files in foundFiles an make pixmaps with a size specified by the given QLabel
void ExifScout::generatePixmaps(const QLabel &labelObject){
    int size = foundFiles.size();
    QString qsize = QString::number(size);
        for(int i=0;i<size;i++){
            Ui_MainWindow::label_showingPicture->setText("Generating previews . . . "+QString::number(i+1)+" of "+qsize);
            Ui_MainWindow::label_showingPicture->show();
            previewPixmaps.insert(foundFiles[i], pixmapFromImage(foundFiles[i], labelObject));
        }
    std::cerr << "previewPixmaps ready!\n";
}


// shows four scaled pixmaps to preview, depending on the scrollbars position
void ExifScout::showImages(int startAtPictureNumber){
if(foundFiles.size()>0) {
    QLabel* label[5] = {Ui_MainWindow::prevImageLabel1,Ui_MainWindow::prevImageLabel2,Ui_MainWindow::prevImageLabel3,Ui_MainWindow::prevImageLabel4,Ui_MainWindow::prevImageLabel_main};

    int countTo = foundFiles.size();
    if(countTo > 4){
        countTo = 4;
    }
    if(startAtPictureNumber < 0){
        startAtPictureNumber = 0;
    }
    if(startAtPictureNumber > foundFiles.size()) {
        startAtPictureNumber = foundFiles.size();
    }
    if(startAtPictureNumber > 4 && startAtPictureNumber > foundFiles.size()-4){
        startAtPictureNumber = foundFiles.size()-4;
    }

    for(int i=0;i<countTo;i++){
        try{
            QPixmap tempPixmap;

            previewPixmaps.find(foundFiles[startAtPictureNumber+i],tempPixmap);
            if(!tempPixmap.isNull()){
                label[i]->setPixmap(tempPixmap);
                label[i]->setWhatsThis(foundFiles[startAtPictureNumber+i]); //save the path for later reconition
                label[i]->show();
            }
            else {               
                std::cerr << "Careful! Pixmap was null! @" << (startAtPictureNumber+i) <<"\n Generate new previewPixmap\n";
                generatePixmaps(*Ui_MainWindow::prevImageLabel1);
            }
        }
        catch(...){
            std::cerr << "Could not load pixmap.\n";
        }
    }
}
else {
    std::cerr << "foundFiles() has not been filled with filenames yet!\n";
}
}


// opens an Image specified by the path and returns its pixmap
QPixmap ExifScout::pixmapFromImage(const QString path, const QLabel &labelObject){
    //std::cerr << "Path is: "<< (std::string)path.toAscii() << "\n";

        QPixmap *pixmap, pixmapScaled;
        pixmap = new QPixmap();

        if(!path.isEmpty()) pixmap->load(path);

        if (pixmap->isNull()) {
                // if no image is found, show the 404 image
                //Ui_MainWindow::statusBar->showMessage("Cannot load "+path, 5000);
                std::cerr << "Cannot load "<< (std::string)path.toAscii();
                pixmap->load("./error404.jpg");
                pixmapScaled = pixmap->scaledToWidth(labelObject.width(), Qt::FastTransformation);
                return pixmapScaled;
        }


            //Scale the pixmap to the size of the label
            if(pixmap->width() > pixmap->height()) {
                pixmapScaled = pixmap->scaledToWidth(labelObject.width(), Qt::FastTransformation);
            }
            else {
                pixmapScaled = pixmap->scaledToHeight(labelObject.height(), Qt::FastTransformation);
            }
                //Ui_MainWindow::statusBar->showMessage("Image opened",5000);
                return pixmapScaled;

}


// shows some information about this program
void ExifScout::about()
{
    QMessageBox::about(this, tr("About Exif-Scout"),
            tr("<p>The <b>EXIF-SCOUT</b></p>"
               "<p>Search and find images by their Exif-Data.</p>"
               "<p>Uses the exiv2-library (<a href='www.exiv2.org'>exiv2.org</a>)</p>"
               "<p>GUI coded by Tim Luedtke (t.luedtke@tu-bs.de)<br />for Technische Universitaet Braunschweig,<br /> Institut for Computer Graphics</p>"
               "<p>Thank you for your excellent help:<br /> S.Br&uuml;ckner, S.Walz, R. Kratou</p>"
               "<p>For more information about the EXIF format see:<br />"
               "<a href='http://en.wikipedia.org/wiki/Exchangeable_image_file_format'>wikipedia.org/Exif</a></p>"));
}
