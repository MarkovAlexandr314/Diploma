

#ifndef PARSER_H
#define PARSER_H
#include "cgnslib.h"
#include <string>
#include <vector>
#include <unordered_map>

#define CGNS_float CGNS_ENUMV(RealSingle)
#define CGNS_double CGNS_ENUMV(RealDouble)

enum COORD
{
    X = 0,
    Y,
    Z
};

template <typename IndexType>
struct InterfaceConnectivity
{
    std::string donor_zone_name;
    std::vector<IndexType> ownerIndex;
    std::vector<IndexType> donorIndex;
};
using Vector3D_Type = std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>;
using ZoneMap_Type = std::unordered_map<std::string, int>;
using Index_Type = std::array<unsigned, 3>;
using InterfaceConnectivity_Type = std::unordered_map<int, std::vector<InterfaceConnectivity<Index_Type>>>;
using FlowSolution3D_Type = std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>>;

class Parser
{
private:
    std::string filename;
    int fn;
    int index_base{1}; // Всегда одна база!!!
    int zone_number{0};
    int dim_number{3}; // Работаем с трех мерными сетками!!!
    Vector3D_Type coords_by_zone;
    ZoneMap_Type zone_name_map;
    InterfaceConnectivity_Type interface_connectivity;
    FlowSolution3D_Type flow_solution;

public:
    Parser(const std::string &);
    void ReadCoordinates();
    void ReadInterfaces();
    void ReadSolution();

    // Геттеры

    // Получить номер зоны
    const int GetZoneNumber();
    const ZoneMap_Type GetZoneNameMap();
    const InterfaceConnectivity_Type GetInterfaceConnectivity();
    const FlowSolution3D_Type GetFlowSolution();

    ~Parser();
};

#endif // PARSER_H