// Inclut une dependance necessaire a ce fichier.
#include "MainWindow.h"

// Inclut une dependance necessaire a ce fichier.
#include <QApplication>
// Inclut une dependance necessaire a ce fichier.
#include <QFont>

// Declare ou utilise une fonction necessaire au programme.
int main(int argc, char* argv[])
// Ouvre un nouveau bloc d instructions.
{
    // Declare ou utilise une fonction necessaire au programme.
    QApplication app(argc, argv);
    // Execute cette instruction dans le flux normal du programme.
    app.setApplicationName("gestion_trafic_rond_point_ngaba");
    // Execute cette instruction dans le flux normal du programme.
    app.setOrganizationName("SITR");
    // Execute cette instruction dans le flux normal du programme.
    app.setFont(QFont("Segoe UI", 10));

    // Execute cette instruction dans le flux normal du programme.
    MainWindow window;
    // Execute cette instruction dans le flux normal du programme.
    window.show();
    // Retourne la valeur calculee par la fonction.
    return app.exec();
// Ferme le bloc d instructions courant.
}
