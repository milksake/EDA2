#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "classes.h"

// VTK
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

void drawOctree(const Octree& tree)
{
    vtkNew<vtkNamedColors> colors;

    // Create a rendering window and renderer.
    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    renWin->SetWindowName("Octree");
    renWin->AddRenderer(ren);
    // Create a renderwindow interactor.
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // Create cubes.
    std::vector<vtkSmartPointer<vtkCubeSource>> cubes;
    tree.processLeaves([&cubes](const Point<int>&, const Point<double>& bBox, double height) {
        cubes.push_back(vtkSmartPointer<vtkCubeSource>::New());
        cubes[cubes.size() - 1]->SetBounds(bBox.x, bBox.x+height, bBox.y, bBox.y+height, bBox.z, bBox.z+height);
    });

    std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
    std::vector<vtkSmartPointer<vtkActor>> actors;
    for (int i = 0; i < cubes.size(); i++)
    {
        cubes[i]->Update();

        // Mapper.
        mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
        mappers[i]->SetInputData(cubes[i]->GetOutput());

        // Actor.
        actors.push_back(vtkSmartPointer<vtkActor>::New());
        actors[i]->SetMapper(mappers[i]);
        actors[i]->GetProperty()->SetColor(colors->GetColor3d("Chocolate").GetData());

        // Assign actor to the renderer.
        ren->AddActor(actors[i]);
    }

    ren->ResetCamera();
    ren->GetActiveCamera()->Azimuth(30);
    ren->GetActiveCamera()->Elevation(30);
    ren->ResetCameraClippingRange();
    ren->SetBackground(colors->GetColor3d("Teal").GetData());

    renWin->SetSize(1000, 600);
    renWin->SetWindowName("Octree");

    // Enable user interface interaction.
    iren->Initialize();
    renWin->Render();
    iren->Start();
}

int main()
{
    std::string datafile("../data/points1.csv");
    std::ifstream file;
    file.open(datafile);

    std::string xyz;
    Point<double> min(0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF);
    Point<double> max(-0xFFFFFFF, -0xFFFFFFF, -0xFFFFFFF);
    while (file >> xyz)
    {
        int c1 = xyz.find_first_of(',');
        int c2 = xyz.find_last_of(',');
        int x = std::stoi(xyz.substr(0, c1));
        if (x < min.x) min.x = x;
        if (x > max.x) max.x = x;
        int y = std::stoi(xyz.substr(c1 + 1, c2));
        if (y < min.y) min.y = y;
        if (y > max.y) max.y = y;
        int z = std::stoi(xyz.substr(c2 + 1));
        if (z < min.z) min.z = z;
        if (z > max.z) max.z = z;
    }
    int dims = std::max(std::max(max.x - min.x, max.y - min.y), max.z - min.z);

    std::cout << "Bottom Left: (" << min.x << ", " << min.y << ", " << min.z << ")\n";
    std::cout << "Height: " << dims << "\n\n";
    
    Octree tree(min, dims);

    file.clear();
    file.seekg(0, std::ios::beg);

    while (file >> xyz)
    {
        int c1 = xyz.find_first_of(',');
        int c2 = xyz.find_last_of(',');
        Point<int> p(
            std::stoi(xyz.substr(0, c1)),
            std::stoi(xyz.substr(c1+1, c2)),
            std::stoi(xyz.substr(c2+1))
        );

        tree.insert(p);
    }

    file.close();

    /* Queries */
    auto qr = tree.find_closest(Point<int>(82, 20, -50));
    std::cout << "Mas cercano a (82, 20, -50): (" << qr.x << ", " << qr.y << ", " << qr.z << ")\n";
    qr = tree.find_closest(Point<int>(12, 112, -8));
    std::cout << "Mas cercano a (12, 112, -8): (" << qr.x << ", " << qr.y << ", " << qr.z << ")\n";
    std::cout << "Existe (207, 304, 46)? " << (tree.exist(Point<int>(207, 304, 46)) ? "Si" : "No") << '\n';
    std::cout << "Existe (105, 108, -26)? " << (tree.exist(Point<int>(105, 108, -26)) ? "Si" : "No") << '\n';

    drawOctree(tree);
    
    return 0;
}