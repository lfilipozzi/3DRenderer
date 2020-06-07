#include "../include/animationwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

/***
 *                     _                 _   _              
 *         /\         (_)               | | (_)             
 *        /  \   _ __  _ _ __ ___   __ _| |_ _  ___  _ __   
 *       / /\ \ | '_ \| | '_ ` _ \ / _` | __| |/ _ \| '_ \  
 *      / ____ \| |_| | | | | |_| | (_| | |_| | (_) | | | | 
 *     /_/    \_\_(_)_|_|_| |_| |_|\__,_|\__|_|\___/|_| |_| 
 *       __      ___ _ __   __| | _____      __             
 *       \ \ /\ / / | '_ \ / _` |/ _ \ \ /\ / /             
 *        \ V  V /| | | | | (_| | (_) \ V  V /              
 *         \_/\_/ |_|_| |_|\__,_|\___/ \_/\_/               
 *                                                          
 *                                                          
 */

AnimationWindow::AnimationWindow(QString envFile, std::vector<QString> vehList) {
    // Menu bar
    QMenu * fileMenu = menuBar()->addMenu("&File");
    QMenu * viewMenu = menuBar()->addMenu("&View");
    QMenu * helpMenu = menuBar()->addMenu("&Help");
    QAction * recordAction = fileMenu->addAction("Export to video");
    QAction * exitAction = fileMenu->addAction("&Exit");
    QAction * toggleSnapshotAction = viewMenu->addAction("&Snapshot mode");
    QAction * toggleGlobFrAction = viewMenu->addAction("Toggle &global frame");
    QAction * toggleTireForceAction = viewMenu->addAction("Toggle &tire forces");
    QAction * aboutAction = helpMenu->addAction("&About");
    
    // Set menu and action options
    fileMenu->insertSeparator(exitAction);
    toggleSnapshotAction->setCheckable(true);
    toggleGlobFrAction->setCheckable(true);
    toggleTireForceAction->setCheckable(true);
    toggleTireForceAction->setChecked(true);
    recordAction->setIcon(
        QIcon::fromTheme("record", QIcon(":/icons/record"))
    );
    exitAction->setIcon(
        QIcon::fromTheme("exit", QIcon(":/icons/exit"))
    );
    aboutAction->setIcon(
        QIcon::fromTheme("help-about", QIcon(":/icons/help-about"))
    );

    // Animation window
    unsigned int refreshRate(30);
    p_openGLWindow = std::make_unique<OpenGLWindow>(
        refreshRate, envFile, vehList
    );
    QWidget * windowContainer = 
        QWidget::createWindowContainer(p_openGLWindow.get());
    windowContainer->setMinimumSize(800, 600);

    // Animation player
    p_player = new AnimationPlayer(p_openGLWindow.get(), this);
    
    // Add the animation and the player to the central widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *vertLayout = new QVBoxLayout(centralWidget);
    vertLayout->addWidget(windowContainer);
    vertLayout->addWidget(p_player);
    vertLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(vertLayout);
    setCentralWidget(centralWidget);
    
    // Create record dialog window
    p_recordDialog = std::make_unique<RecordDialog>(p_openGLWindow.get());
    
    connect(toggleSnapshotAction, SIGNAL(triggered()),
            p_openGLWindow.get(), SLOT(toggleSnapshotMode()));
    connect(recordAction, SIGNAL(triggered()), 
            p_recordDialog.get(), SLOT(show()));
    connect(exitAction, SIGNAL(triggered()), 
            qApp, SLOT(quit()));
    connect(toggleGlobFrAction, SIGNAL(triggered()), 
            p_openGLWindow.get(), SLOT(toggleGlobalFrame()));
    connect(toggleTireForceAction, SIGNAL(triggered()), 
            p_openGLWindow.get(), SLOT(toggleTireForce()));
    connect(aboutAction, SIGNAL(triggered()), 
            this, SLOT(openAboutWindow()));
}


AnimationWindow::~AnimationWindow() {
    
}


void AnimationWindow::openAboutWindow() {
    QMessageBox::information(
        this, "About", 
        "3D viewer.\n\nAuthor: Louis Filipozzi\n\n"
        "Dependencies: Assimp, Qt, OpenGL, ffmpeg"
    );
}


/***
 *      _____                        _            
 *     |  __ \                      | |           
 *     | |__) |___  ___ ___  _ __ __| | ___ _ __  
 *     |  _  // _ \/ __/ _ \| '__/ _` |/ _ \ '__| 
 *     | | \ \  __/ (_| (_) | | | (_| |  __/ |    
 *     |_|  \_\___|\___\___/|_|  \__,_|\___|_|    
 *            | (_)     | |                       
 *          __| |_  __ _| | ___   __ _            
 *         / _` | |/ _` | |/ _ \ / _` |           
 *        | (_| | | (_| | | (_) | (_| |           
 *         \__,_|_|\__,_|_|\___/ \__, |           
 *                                __/ |           
 *                               |___/            
 */

#include <QLabel>

RecordDialog::RecordDialog(OpenGLWindow * window, QWidget * parent) 
: QWidget(parent), p_openGLWindow(window) {
    QVBoxLayout * vertLayout = new QVBoxLayout(this);
    QGridLayout * gridLayout = new QGridLayout();
    QHBoxLayout * horiLayout = new QHBoxLayout();
    vertLayout->addLayout(gridLayout);
    vertLayout->addLayout(horiLayout);
    
    QLabel * fileNameLabel = new QLabel(tr("Output file"), this);
    p_fileNameLineEdit = new QLineEdit(this);
    QPushButton * fileNameButton = new QPushButton(this);
    QLabel * fpsLabel = new QLabel(tr("Frame rate"), this);
    p_fpsSpinBox = new QSpinBox(this);
    QLabel * resolutionLabel = new QLabel(tr("Resolution"), this);
    p_resolutionComboBox = new QComboBox(this);
    QPushButton * exportButton = new QPushButton("Export", this);
    QPushButton * cancelButton = new QPushButton("Cancel", this);
    
    fileNameButton->setIcon(
        QIcon::fromTheme("folder-saved-search", QIcon(":/icons/saveFolder"))
    );
    p_fileNameLineEdit->setText("output/video.mp4");
    p_fpsSpinBox->setValue(30);
    p_resolutionComboBox->addItem("480p");
    p_resolutionComboBox->addItem("540p");
    p_resolutionComboBox->addItem("720p");
    p_resolutionComboBox->addItem("1080p");
    p_resolutionComboBox->setCurrentText("720p");
    exportButton->setIcon(
        QIcon::fromTheme("poedit-validate", QIcon(":/icons/validate"))
    );
    cancelButton->setIcon(
        QIcon::fromTheme("cancel", QIcon(":/icons/cancel"))
    );
    
    gridLayout->addWidget(fileNameLabel, 0, 0);
    gridLayout->addWidget(p_fileNameLineEdit, 0, 1);
    gridLayout->addWidget(fileNameButton, 0 , 2);
    gridLayout->addWidget(fpsLabel, 1, 0);
    gridLayout->addWidget(p_fpsSpinBox, 1, 1);
    gridLayout->addWidget(resolutionLabel, 2, 0);
    gridLayout->addWidget(p_resolutionComboBox, 2, 1);
    
    horiLayout->addWidget(exportButton);
    horiLayout->addWidget(cancelButton);
    
    setLayout(vertLayout);
    
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Video recorder");
    
    connect(fileNameButton, SIGNAL(clicked()), this, SLOT(setFileName()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(record()));
}


RecordDialog::~RecordDialog() {
    
}


void RecordDialog::setFileName() {
    // Open browser
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Video File"), 
        QDir::currentPath(), tr("Video Files (*.mp4)")
    );
    
    // Change the filename
    if (!fileName.isEmpty()) {
        p_fileNameLineEdit->setText(fileName);
    }
}


void RecordDialog::record() {
    QString fileName = p_fileNameLineEdit->text();
    int fps = p_fpsSpinBox->value();
    int height = 480;
    int width = 720;
    if (QString::compare(p_resolutionComboBox->currentText(), "480p") == 0) {
        height = 480;
        width = 720;
    }
    else if (QString::compare(p_resolutionComboBox->currentText(), "540p") == 0) {
        height = 540;
        width = 960;
    }
    else if (QString::compare(p_resolutionComboBox->currentText(), "720p") == 0) {
        height = 720;
        width = 1280;
    }
    else if (QString::compare(p_resolutionComboBox->currentText(), "1080p") == 0) {
        height = 1080;
        width = 1920;
    }
    
    if (p_openGLWindow != nullptr)
        p_openGLWindow->record(fps, width, height, fileName);
    
    close();
}








