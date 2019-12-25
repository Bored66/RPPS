// Description: Настройки генерации кода

#ifndef GENERATORSETTINGS
#define GENERATORSETTINGS

#include <QStringList>

struct GeneratorSettings
{
    bool GenerateClient;
    bool GenerateServer;
    bool GenerateTest;
    bool RemoveFilesBeforeCreating;
    bool FilterOutput;
    bool IgnoreOuputDirFromProject;
    bool RelativeToChosen;
    QStringList OutputPath;
    int CoreNumber;
};
#endif // GENERATORSETTINGS

