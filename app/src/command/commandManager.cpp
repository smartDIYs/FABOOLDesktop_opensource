#include <QObject>
#include <QFileDialog>

#include "commandManager.h"
#include "common/dataManager.h"


CommandManager::CommandManager(MainScene* scene, QObject* parent):
    QObject(parent)
{
    mLookingScene  = scene;
    mUndoStack     = new QUndoStack();
    mUndoStack->setUndoLimit(COMMAND_UNDO_STACK_MAX);
    mInstance = DataManager::getInstance();
}

CommandManager::~CommandManager()
{
    mUndoStack->clear();
    delete mUndoStack;
}

void CommandManager::importCommnad()
{
    static QString memoriedPath = "/";
    QStringList fileList = QFileDialog::getOpenFileNames(NULL, tr("Open Image Files"), memoriedPath,
                           "images (*.png *.jpeg *.jpg *.bmp *.gif *.tiff *.tif *.svg *.dxf)");
    if(fileList.count() == 0) { return ; }
    int pathIndx = fileList[0].lastIndexOf("/");
    memoriedPath = fileList[0].left(pathIndx);

    QMap<QString, EN_LOAD_STATE> resultLoad;
    QList<ItemInfo*> items = mInstance->loadImage(fileList, &resultLoad);

    bool isExistNotice = false;
    QMap<QString, QString> errImg;
    foreach (QString key, resultLoad.keys()) {
        if(resultLoad.value(key) != EN_LOAD_SUCCESS) {
            errImg.insert(key, ItemInfo::getLoadStateStr( resultLoad.value(key) ));
            isExistNotice = true;
        }
    }
    if(isExistNotice == true) {
        emit loadErr(errImg);
    }

    if(items.count() <= 0) {
        return;
    }
    mUndoStack->push(new ImportCommand(mLookingScene, items));
    mInstance->modified();
    emit update();
}

void CommandManager::importCommnad(QStringList fileList)
{
    if(fileList.count() == 0) { return ; }

    QMap<QString, EN_LOAD_STATE> resultLoad;
    QList<ItemInfo*> items = mInstance->loadImage(fileList, &resultLoad);

    bool isExistNotice = false;
    QMap<QString, QString> errImg;
    foreach (QString key, resultLoad.keys()) {
        if(resultLoad.value(key) != EN_LOAD_SUCCESS) {
            errImg.insert(key, ItemInfo::getLoadStateStr( resultLoad.value(key) ));
            isExistNotice = true;
        }
    }
    if(isExistNotice == true) {
        emit loadErr(errImg);
    }

    if(items.count() <= 0) {
        return;
    }

    mUndoStack->push(new ImportCommand(mLookingScene, items));
    mInstance->modified();
    emit update();
}

void CommandManager::deleteCommand()
{
    if(mLookingScene->selectedFlexFrame().count() == 0) { return ; }
    mUndoStack->push(new DeleteCommand(mLookingScene));
    mInstance->modified();
    emit update();
}

void CommandManager::deleteAllCommand()
{
    if(mInstance->ImageItem().count() == 0) { return ; }
    mLookingScene->allSelect();
    mUndoStack->push(new DeleteCommand(mLookingScene));
    mInstance->modified();
    emit update();
}

void CommandManager::pasteCommnad()
{
    if(mLookingScene->copyList().count() == 0) { return; }
    mUndoStack->push(new PasteCommand(mLookingScene));
    mInstance->modified();
    emit update();
}

void CommandManager::pasteCommnad(QPointF pos)
{
    if(mLookingScene->copyList().count() == 0) { return; }
    mUndoStack->push(new PasteCommand(mLookingScene, pos));
    mInstance->modified();
    emit update();
}

void CommandManager::shapeChangeCommand(bool isHead)
{
    if(mLookingScene->selectedFlexFrame().count() == 0) { return ; }
    mUndoStack->push(new ShapeChangeCommand(mLookingScene, isHead));
    mInstance->modified();
}

void CommandManager::groupCommand()
{
    if(mLookingScene->hasGroup() == true) { return; }
    //ラスタ画像はGroup化をはじく
    foreach (GraphicsFlexibleFrame* frame, mLookingScene->selectedFlexFrame()) {
        if(frame->isRaster() == true) {
            frame->setSelected(false);
        }
    }

    if(mLookingScene->selectedFlexFrame().count() == 0) { return ; }
    mUndoStack->push(new GroupCommand(mLookingScene));
    mInstance->modified();
    emit update();
}

void CommandManager::ungroupCommand()
{
    if(mLookingScene->hasGroup() == false) { return; }
    mUndoStack->push(new UngroupCommand(mLookingScene));
    mInstance->modified();
    emit update();
}

