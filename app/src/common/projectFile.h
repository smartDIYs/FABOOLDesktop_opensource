#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "common/common.h"

// プロジェクトファイルの読み書きを担当
// スタティック関数で構成されたクラス

//　XmlでDom形式にして保存する

class ProjectFile
{
public:
    ProjectFile();
    ~ProjectFile() {;}

    static int save();                  // ロードされたプロジェクトを保存
    static int saveAs(QString path);    //　新規作成されたプロジェクト or 名前を付けて保存
    static int load(QString path);      // ロード
};

#endif // PROJECTFILE_H
