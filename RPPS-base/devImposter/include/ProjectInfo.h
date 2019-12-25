// Description: Структура настроек проекта
#ifndef PROJECTINFO
#define PROJECTINFO

#include <QStringList>

struct ProjectInfo
{
    QStringList                             colFormats;
    QStringList                             colCommands;
    QString                                 outputDir;
    QString                                 name;
    QStringList                             colIncludePathList;
    QString                                 libPath;
    void                                    Reset()
    {
        colCommands.clear();
        colFormats.clear();
        colIncludePathList.clear();
        outputDir.clear();
        libPath.clear();
    }
    const ProjectInfo&               operator= ( const struct ProjectInfo& from )
    {
        Reset();
        name = from.name;
        colCommands.append( from.colCommands );
        colFormats.append( from.colFormats );
        colIncludePathList.append( from.colIncludePathList );
        outputDir = from.outputDir;
        libPath = from.libPath;
        return *this;
    }
    ProjectInfo()
    { Reset(); }
};

#endif // PROJECTINFO

