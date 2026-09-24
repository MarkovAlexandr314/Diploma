#include <cgnslib.h>
#include <stdio.h>
#include <malloc.h>
#include "Parser.h"
#include <iostream>

int CG_MAX_NAME_LENGTH = 40;

int main()
{

    Parser CGNS_Parser("/home/alex/Diploma/CGNS/example/xyz-L-cut4-z1-ordered-T4.cgns");
    // Parser CGNS_Parser("structured2D.cgns");

    CGNS_Parser.ReadCoordinates();
    std::cout << "ReadCoordinates succesfully!!!" << std::endl;
    std::cout << "Zone numbers: " << CGNS_Parser.GetZoneNumber() << std::endl;
    auto um = CGNS_Parser.GetZoneNameMap();
    std::cout << "=========== Zones by Name =====================" << std::endl;
    for (auto it = um.begin(); it != um.end(); it++)
        std::cout << it->first << ": " << it->second
                  << std::endl;

    CGNS_Parser.ReadInterfaces();
    std::cout << "ReadInterfaces succesfully!!!" << std::endl;

    InterfaceConnectivity_Type interfaces = CGNS_Parser.GetInterfaceConnectivity();
    for (auto interface : interfaces[2])
    {
        std::cout << "Zone-donor name: " << interface.donor_zone_name << std::endl;
        std::cout << "Donor: " << std::endl;
        for (size_t i = 0; i < interface.donorIndex.size(); i++)
        {
            std::cout << "X: " << interface.donorIndex[i][0] << ' ' << "Y: " << interface.donorIndex[i][1] << ' ' << "Z: " << interface.donorIndex[i][2] << ' ' << std::endl;
        }
        std::cout << "Owner: " << std::endl;
        for (size_t i = 0; i < interface.ownerIndex.size(); i++)
        {
            std::cout << "X: " << interface.ownerIndex[i][0] << ' ' << "Y: " << interface.ownerIndex[i][1] << ' ' << "Z: " << interface.ownerIndex[i][2] << ' ' << std::endl;
        }
    }

    std::cout << "=================== Read Flow Solution ===================" << std::endl;
    CGNS_Parser.ReadSolution();
    std::cout << "Coord: " << CGNS_Parser.GetCoordinates()[0].size() << std::endl;
    std::cout << "Flow: " << CGNS_Parser.GetFlowSolution()[0][0].size() << std::endl;
}